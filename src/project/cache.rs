use std::collections::BTreeMap;
use std::fmt;
use std::fs::{self, File, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicU64, Ordering};

use crate::incremental::Fingerprint;

const MAGIC: &[u8] = b"SLIMCACHE\0";
const SCHEMA: u32 = 1;
const MAX_IDENTITY_BYTES: usize = 4 * 1024;
const MAX_VERSION_BYTES: usize = 1024;
const MAX_DEPENDENCIES: usize = 100_000;
const MAX_ARTIFACT_BYTES: usize = 64 * 1024 * 1024;
const MIN_FIXED_BYTES: usize = MAGIC.len() + 4 + 4 + 4 + 8 + 8 + 4 + 4 + 4 + 4 + 8;

static NEXT_TEMPORARY: AtomicU64 = AtomicU64::new(0);

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct ModuleCacheEntry {
    pub module: String,
    pub source: Fingerprint,
    pub interface_fingerprint: Fingerprint,
    pub declaration_count: u32,
    pub dependencies: BTreeMap<String, Fingerprint>,
    pub interface: String,
    pub generated_c: String,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct CacheError(String);

impl CacheError {
    fn new(message: impl Into<String>) -> Self {
        Self(message.into())
    }
}

impl fmt::Display for CacheError {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        formatter.write_str(&self.0)
    }
}

pub fn fingerprint(bytes: &[u8]) -> Fingerprint {
    let mut value = 0xcbf29ce484222325_u64;
    for byte in bytes {
        value ^= u64::from(*byte);
        value = value.wrapping_mul(0x100000001b3);
    }
    Fingerprint(value)
}

pub fn file_name(module: &str) -> String {
    fingerprint(module.as_bytes()).to_string()
}

pub fn encode(entry: &ModuleCacheEntry) -> Result<Vec<u8>, CacheError> {
    validate_encode(entry)?;
    let mut output = Vec::new();
    output.extend_from_slice(MAGIC);
    push_u32(&mut output, SCHEMA);
    push_bytes(&mut output, crate::VERSION.as_bytes());
    push_bytes(&mut output, entry.module.as_bytes());
    push_u64(&mut output, entry.source.0);
    push_u64(&mut output, entry.interface_fingerprint.0);
    push_u32(&mut output, entry.declaration_count);
    push_u32(
        &mut output,
        u32::try_from(entry.dependencies.len())
            .map_err(|_| CacheError::new("too many cache dependencies"))?,
    );
    for (module, interface) in &entry.dependencies {
        push_bytes(&mut output, module.as_bytes());
        push_u64(&mut output, interface.0);
    }
    push_bytes(&mut output, entry.interface.as_bytes());
    push_bytes(&mut output, entry.generated_c.as_bytes());
    let checksum = fingerprint(&output);
    push_u64(&mut output, checksum.0);
    Ok(output)
}

pub fn decode(bytes: &[u8], expected_module: &str) -> Result<ModuleCacheEntry, CacheError> {
    if bytes.len() < MIN_FIXED_BYTES {
        return Err(CacheError::new("truncated cache entry"));
    }
    let (payload, checksum_bytes) = bytes.split_at(bytes.len() - 8);
    let expected_checksum = read_u64_exact(checksum_bytes);
    if fingerprint(payload).0 != expected_checksum {
        return Err(CacheError::new("cache checksum mismatch"));
    }

    let mut reader = Reader::new(payload);
    if reader.take(MAGIC.len())? != MAGIC {
        return Err(CacheError::new("invalid cache magic"));
    }
    if reader.u32()? != SCHEMA {
        return Err(CacheError::new("unsupported cache schema"));
    }
    let version = reader.string(MAX_VERSION_BYTES, "compiler version")?;
    if version != crate::VERSION {
        return Err(CacheError::new("cache compiler version mismatch"));
    }
    let module = reader.string(MAX_IDENTITY_BYTES, "module identity")?;
    if module != expected_module {
        return Err(CacheError::new("cache module identity mismatch"));
    }
    let source = Fingerprint(reader.u64()?);
    let interface_fingerprint = Fingerprint(reader.u64()?);
    let declaration_count = reader.u32()?;
    let dependency_count = usize::try_from(reader.u32()?)
        .map_err(|_| CacheError::new("cache dependency count overflow"))?;
    if dependency_count > MAX_DEPENDENCIES {
        return Err(CacheError::new("cache dependency count exceeds limit"));
    }
    let mut dependencies = BTreeMap::new();
    let mut previous: Option<String> = None;
    for _ in 0..dependency_count {
        let dependency = reader.string(MAX_IDENTITY_BYTES, "dependency identity")?;
        if previous.as_ref().is_some_and(|prior| prior >= &dependency) {
            return Err(CacheError::new(
                "cache dependencies are duplicate or not strictly sorted",
            ));
        }
        previous = Some(dependency.clone());
        dependencies.insert(dependency, Fingerprint(reader.u64()?));
    }
    let interface = reader.string(MAX_ARTIFACT_BYTES, "interface artifact")?;
    let generated_c = reader.string(MAX_ARTIFACT_BYTES, "generated C")?;
    if !reader.is_empty() {
        return Err(CacheError::new("cache entry contains excess bytes"));
    }
    if fingerprint(interface.as_bytes()) != interface_fingerprint {
        return Err(CacheError::new("cached interface fingerprint mismatch"));
    }
    if !super::interface::validate_artifact(&module, &interface) {
        return Err(CacheError::new("cached interface artifact is malformed"));
    }
    Ok(ModuleCacheEntry {
        module,
        source,
        interface_fingerprint,
        declaration_count,
        dependencies,
        interface,
        generated_c,
    })
}

pub fn read(directory: &Path, module: &str) -> Result<Option<ModuleCacheEntry>, CacheError> {
    let path = directory.join(file_name(module));
    let bytes = match fs::read(path) {
        Ok(bytes) => bytes,
        Err(error) if error.kind() == io::ErrorKind::NotFound => return Ok(None),
        Err(error) => return Err(CacheError::new(format!("cannot read cache entry: {error}"))),
    };
    decode(&bytes, module).map(Some)
}

pub fn write_atomic(directory: &Path, entry: &ModuleCacheEntry) -> Result<PathBuf, CacheError> {
    let bytes = encode(entry)?;
    fs::create_dir_all(directory)
        .map_err(|error| CacheError::new(format!("cannot create cache directory: {error}")))?;
    let destination = directory.join(file_name(&entry.module));
    let (temporary, mut file) = create_temporary(directory, &entry.module).map_err(|error| {
        CacheError::new(format!("cannot create temporary cache entry: {error}"))
    })?;
    let result = (|| -> io::Result<()> {
        file.write_all(&bytes)?;
        file.sync_all()?;
        drop(file);
        fs::rename(&temporary, &destination)?;
        Ok(())
    })();
    if let Err(error) = result {
        let _ = fs::remove_file(&temporary);
        return Err(CacheError::new(format!(
            "cannot commit cache entry: {error}"
        )));
    }
    Ok(destination)
}

fn validate_encode(entry: &ModuleCacheEntry) -> Result<(), CacheError> {
    if entry.module.len() > MAX_IDENTITY_BYTES {
        return Err(CacheError::new("cache module identity exceeds limit"));
    }
    if crate::VERSION.len() > MAX_VERSION_BYTES {
        return Err(CacheError::new("compiler version exceeds cache limit"));
    }
    if entry.dependencies.len() > MAX_DEPENDENCIES {
        return Err(CacheError::new("cache dependency count exceeds limit"));
    }
    if entry
        .dependencies
        .keys()
        .any(|dependency| dependency.len() > MAX_IDENTITY_BYTES)
    {
        return Err(CacheError::new("cache dependency identity exceeds limit"));
    }
    if entry.interface.len() > MAX_ARTIFACT_BYTES || entry.generated_c.len() > MAX_ARTIFACT_BYTES {
        return Err(CacheError::new("cache artifact exceeds limit"));
    }
    if fingerprint(entry.interface.as_bytes()) != entry.interface_fingerprint {
        return Err(CacheError::new(
            "interface fingerprint does not match bytes",
        ));
    }
    if !super::interface::validate_artifact(&entry.module, &entry.interface) {
        return Err(CacheError::new("interface artifact is malformed"));
    }
    Ok(())
}

fn create_temporary(directory: &Path, module: &str) -> io::Result<(PathBuf, File)> {
    for _ in 0..1000 {
        let serial = NEXT_TEMPORARY.fetch_add(1, Ordering::Relaxed);
        let path = directory.join(format!(
            ".{}.{}.{serial}.tmp",
            file_name(module),
            std::process::id()
        ));
        match OpenOptions::new().write(true).create_new(true).open(&path) {
            Ok(file) => return Ok((path, file)),
            Err(error) if error.kind() == io::ErrorKind::AlreadyExists => continue,
            Err(error) => return Err(error),
        }
    }
    Err(io::Error::new(
        io::ErrorKind::AlreadyExists,
        "cannot allocate unique temporary cache path",
    ))
}

fn push_u32(output: &mut Vec<u8>, value: u32) {
    output.extend_from_slice(&value.to_be_bytes());
}

fn push_u64(output: &mut Vec<u8>, value: u64) {
    output.extend_from_slice(&value.to_be_bytes());
}

fn push_bytes(output: &mut Vec<u8>, value: &[u8]) {
    push_u32(
        output,
        u32::try_from(value.len()).expect("validated cache field length fits u32"),
    );
    output.extend_from_slice(value);
}

fn read_u64_exact(bytes: &[u8]) -> u64 {
    u64::from_be_bytes(bytes.try_into().expect("checksum is exactly eight bytes"))
}

struct Reader<'a> {
    bytes: &'a [u8],
    offset: usize,
}

impl<'a> Reader<'a> {
    fn new(bytes: &'a [u8]) -> Self {
        Self { bytes, offset: 0 }
    }

    fn take(&mut self, length: usize) -> Result<&'a [u8], CacheError> {
        let end = self
            .offset
            .checked_add(length)
            .ok_or_else(|| CacheError::new("cache length overflow"))?;
        let value = self
            .bytes
            .get(self.offset..end)
            .ok_or_else(|| CacheError::new("truncated cache field"))?;
        self.offset = end;
        Ok(value)
    }

    fn u32(&mut self) -> Result<u32, CacheError> {
        let bytes: [u8; 4] = self
            .take(4)?
            .try_into()
            .map_err(|_| CacheError::new("truncated cache integer"))?;
        Ok(u32::from_be_bytes(bytes))
    }

    fn u64(&mut self) -> Result<u64, CacheError> {
        let bytes: [u8; 8] = self
            .take(8)?
            .try_into()
            .map_err(|_| CacheError::new("truncated cache integer"))?;
        Ok(u64::from_be_bytes(bytes))
    }

    fn string(&mut self, limit: usize, role: &str) -> Result<String, CacheError> {
        let length = usize::try_from(self.u32()?)
            .map_err(|_| CacheError::new(format!("{role} length overflow")))?;
        if length > limit {
            return Err(CacheError::new(format!("{role} exceeds cache limit")));
        }
        let bytes = self.take(length)?;
        std::str::from_utf8(bytes)
            .map(str::to_owned)
            .map_err(|_| CacheError::new(format!("{role} is not valid UTF-8")))
    }

    fn is_empty(&self) -> bool {
        self.offset == self.bytes.len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn entry() -> ModuleCacheEntry {
        let interface = "(interface 1 math (fn answer () I64 (effects)))\n".to_owned();
        ModuleCacheEntry {
            module: "math".to_owned(),
            source: Fingerprint(7),
            interface_fingerprint: fingerprint(interface.as_bytes()),
            declaration_count: 2,
            dependencies: BTreeMap::from([
                ("base".to_owned(), Fingerprint(11)),
                ("types".to_owned(), Fingerprint(12)),
            ]),
            interface,
            generated_c: "static int64_t slim_math_answer(void) { return 42; }\n".to_owned(),
        }
    }

    fn replace_payload_byte(mut bytes: Vec<u8>, offset: usize, value: u8) -> Vec<u8> {
        bytes[offset] = value;
        let checksum_offset = bytes.len() - 8;
        let checksum = fingerprint(&bytes[..checksum_offset]);
        bytes[checksum_offset..].copy_from_slice(&checksum.0.to_be_bytes());
        bytes
    }

    #[test]
    fn round_trips_and_uses_stable_identity_file_name() {
        let entry = entry();
        let bytes = encode(&entry).unwrap();
        assert_eq!(decode(&bytes, "math").unwrap(), entry);
        assert_eq!(file_name("math"), file_name("math"));
        assert_ne!(file_name("math"), file_name("other"));
    }

    #[test]
    fn rejects_every_truncation_and_every_single_byte_mutation() {
        let bytes = encode(&entry()).unwrap();
        for end in 0..bytes.len() {
            assert!(decode(&bytes[..end], "math").is_err(), "truncation {end}");
        }
        for index in 0..bytes.len() {
            let mut mutated = bytes.clone();
            mutated[index] ^= 0x5a;
            assert!(decode(&mutated, "math").is_err(), "mutation {index}");
        }
    }

    #[test]
    fn rejects_schema_version_identity_and_excess_bytes() {
        let bytes = encode(&entry()).unwrap();
        let schema_offset = MAGIC.len() + 3;
        assert!(
            decode(
                &replace_payload_byte(bytes.clone(), schema_offset, 2),
                "math"
            )
            .is_err()
        );
        assert!(decode(&bytes, "other").is_err());

        let mut excess = bytes[..bytes.len() - 8].to_vec();
        excess.push(0);
        let checksum = fingerprint(&excess);
        excess.extend_from_slice(&checksum.0.to_be_bytes());
        assert!(decode(&excess, "math").is_err());
    }

    #[test]
    fn writes_and_reads_atomic_entries() {
        let directory = std::env::temp_dir().join(format!(
            "slim-cache-test-{}-{}",
            std::process::id(),
            NEXT_TEMPORARY.fetch_add(1, Ordering::Relaxed)
        ));
        let _ = fs::remove_dir_all(&directory);
        let entry = entry();
        let path = write_atomic(&directory, &entry).unwrap();
        assert_eq!(path.file_name().unwrap(), file_name("math").as_str());
        assert_eq!(read(&directory, "math").unwrap(), Some(entry));
        assert_eq!(read(&directory, "absent").unwrap(), None);
        fs::remove_dir_all(directory).unwrap();
    }
}
