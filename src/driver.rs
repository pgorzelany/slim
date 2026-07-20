use std::env;
use std::ffi::OsString;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::process::{Command, ExitStatus};
use std::time::{SystemTime, UNIX_EPOCH};

use crate::compiler::{self, Compilation};
use crate::diagnostic::Diagnostic;
use crate::sema::Builtin;
use crate::span::Source;

const RUNTIME_C: &str = include_str!("../runtime/slim_rt.c");
const RUNTIME_H: &str = include_str!("../runtime/slim_rt.h");

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum MessageFormat {
    Human,
    Json,
}

pub fn run(arguments: impl IntoIterator<Item = OsString>) -> i32 {
    match run_inner(arguments.into_iter().collect()) {
        Ok(code) => code,
        Err(error) => {
            eprintln!("slimc: {error}");
            2
        }
    }
}

fn run_inner(arguments: Vec<OsString>) -> Result<i32, String> {
    let mut args = Arguments::new(arguments);
    let _program = args.next();
    let mut message_format = MessageFormat::Human;
    while let Some(argument) = args.peek_string() {
        if argument == "--message-format=json" {
            message_format = MessageFormat::Json;
            args.next();
        } else if argument == "--message-format=human" {
            args.next();
        } else {
            break;
        }
    }
    let Some(command) = args.next_string() else {
        print_usage();
        return Ok(64);
    };
    match command.as_str() {
        "check" => {
            let path = required_path(&mut args, "source file")?;
            args.finish()?;
            let compilation = compile_path(&path)?;
            render_diagnostics(&compilation, message_format);
            Ok(if compilation.succeeded() { 0 } else { 1 })
        }
        "emit-c" => {
            let source_path = required_path(&mut args, "source file")?;
            let output = parse_output(&mut args)?;
            args.finish()?;
            let compilation = compile_path(&source_path)?;
            render_diagnostics(&compilation, message_format);
            let Some(c) = compilation.emit_c() else {
                return Ok(1);
            };
            write_file(&output, c.as_bytes())?;
            Ok(0)
        }
        "build" => {
            let source_path = required_path(&mut args, "source file")?;
            let output =
                parse_optional_output(&mut args)?.unwrap_or_else(|| default_output(&source_path));
            args.finish()?;
            let compilation = compile_path(&source_path)?;
            render_diagnostics(&compilation, message_format);
            let Some(c) = compilation.emit_c() else {
                return Ok(1);
            };
            let build = NativeBuild::new()?;
            let status = build.compile(&c, &output)?;
            if !status.success() {
                return Ok(status.code().unwrap_or(2));
            }
            Ok(0)
        }
        "run" => {
            let source_path = required_path(&mut args, "source file")?;
            let program_arguments = if args.peek_string().as_deref() == Some("--") {
                args.next();
                args.remaining()
            } else {
                args.finish()?;
                Vec::new()
            };
            let compilation = compile_path(&source_path)?;
            render_diagnostics(&compilation, message_format);
            let Some(c) = compilation.emit_c() else {
                return Ok(1);
            };
            let build = NativeBuild::new()?;
            let executable = build.path.join("program");
            let status = build.compile(&c, &executable)?;
            if !status.success() {
                return Ok(status.code().unwrap_or(2));
            }
            let status = Command::new(&executable)
                .args(program_arguments)
                .status()
                .map_err(|error| format!("cannot execute {}: {error}", executable.display()))?;
            Ok(status.code().unwrap_or(2))
        }
        "fmt" => {
            let path = required_path(&mut args, "source file")?;
            let check_only = args.peek_string().as_deref() == Some("--check");
            if check_only {
                args.next();
            }
            args.finish()?;
            let source = read_source(&path)?;
            match compiler::format_source(&source) {
                Ok(formatted) if check_only && formatted != source.text => {
                    eprintln!("{} is not canonically formatted", path.display());
                    Ok(1)
                }
                Ok(_) if check_only => Ok(0),
                Ok(formatted) => {
                    write_file(&path, formatted.as_bytes())?;
                    Ok(0)
                }
                Err(diagnostics) => {
                    render_diagnostic_list(&source, &diagnostics, message_format);
                    Ok(1)
                }
            }
        }
        "runtime" => {
            let directory = required_path(&mut args, "output directory")?;
            args.finish()?;
            fs::create_dir_all(&directory)
                .map_err(|error| format!("cannot create {}: {error}", directory.display()))?;
            write_file(&directory.join("slim_rt.c"), RUNTIME_C.as_bytes())?;
            write_file(&directory.join("slim_rt.h"), RUNTIME_H.as_bytes())?;
            Ok(0)
        }
        "builtins" => {
            args.finish()?;
            print_builtins();
            Ok(0)
        }
        "--version" | "version" => {
            println!("slimc {}", crate::VERSION);
            Ok(0)
        }
        "--help" | "help" => {
            print_usage();
            Ok(0)
        }
        _ => Err(format!("unknown command `{command}`; run `slimc help`")),
    }
}

fn compile_path(path: &Path) -> Result<Compilation, String> {
    Ok(compiler::compile(read_source(path)?))
}

fn read_source(path: &Path) -> Result<Source, String> {
    let text = fs::read_to_string(path)
        .map_err(|error| format!("cannot read {} as UTF-8: {error}", path.display()))?;
    Ok(Source::new(path, text))
}

fn render_diagnostics(compilation: &Compilation, format: MessageFormat) {
    render_diagnostic_list(&compilation.source, &compilation.diagnostics, format);
}

fn render_diagnostic_list(source: &Source, diagnostics: &[Diagnostic], format: MessageFormat) {
    for diagnostic in diagnostics {
        match format {
            MessageFormat::Human => eprint!("{}", diagnostic.render_human(source)),
            MessageFormat::Json => eprintln!("{}", diagnostic.render_json(source)),
        }
    }
}

fn parse_output(args: &mut Arguments) -> Result<PathBuf, String> {
    parse_optional_output(args)?.ok_or_else(|| "missing required `-o <output>`".to_owned())
}

fn parse_optional_output(args: &mut Arguments) -> Result<Option<PathBuf>, String> {
    if args.peek_string().as_deref() != Some("-o") {
        return Ok(None);
    }
    args.next();
    Ok(Some(required_path(args, "output path")?))
}

fn required_path(args: &mut Arguments, role: &str) -> Result<PathBuf, String> {
    args.next()
        .map(PathBuf::from)
        .ok_or_else(|| format!("missing required {role}"))
}

fn default_output(source: &Path) -> PathBuf {
    let mut output = source.to_path_buf();
    output.set_extension("");
    output
}

fn write_file(path: &Path, contents: &[u8]) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent)
            .map_err(|error| format!("cannot create {}: {error}", parent.display()))?;
    }
    fs::write(path, contents).map_err(|error| format!("cannot write {}: {error}", path.display()))
}

struct NativeBuild {
    path: PathBuf,
}

impl NativeBuild {
    fn new() -> Result<Self, String> {
        let nonce = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .map_err(|error| format!("system clock error: {error}"))?
            .as_nanos();
        let path = env::temp_dir().join(format!("slim-build-{}-{nonce}", std::process::id()));
        fs::create_dir(&path).map_err(|error| {
            format!("cannot create build directory {}: {error}", path.display())
        })?;
        write_file(&path.join("slim_rt.c"), RUNTIME_C.as_bytes())?;
        write_file(&path.join("slim_rt.h"), RUNTIME_H.as_bytes())?;
        Ok(Self { path })
    }

    fn compile(&self, generated_c: &str, output: &Path) -> Result<ExitStatus, String> {
        let generated_path = self.path.join("program.c");
        write_file(&generated_path, generated_c.as_bytes())?;
        let compiler = env::var_os("CC").unwrap_or_else(|| OsString::from("clang"));
        Command::new(&compiler)
            .arg("-std=c11")
            .arg("-O2")
            .arg("-Wall")
            .arg("-Wextra")
            .arg("-Werror")
            .arg("-I")
            .arg(&self.path)
            .arg(&generated_path)
            .arg(self.path.join("slim_rt.c"))
            .arg("-o")
            .arg(output)
            .status()
            .map_err(|error| format!("cannot execute native compiler {:?}: {error}", compiler))
    }
}

impl Drop for NativeBuild {
    fn drop(&mut self) {
        if let Err(error) = fs::remove_dir_all(&self.path)
            && error.kind() != io::ErrorKind::NotFound
        {
            eprintln!(
                "slimc warning: cannot remove build directory {}: {error}",
                self.path.display()
            );
        }
    }
}

struct Arguments {
    values: Vec<OsString>,
    index: usize,
}

impl Arguments {
    fn new(values: Vec<OsString>) -> Self {
        Self { values, index: 0 }
    }

    fn next(&mut self) -> Option<OsString> {
        let value = self.values.get(self.index).cloned();
        self.index += usize::from(value.is_some());
        value
    }

    fn next_string(&mut self) -> Option<String> {
        self.next()?.into_string().ok()
    }

    fn peek_string(&self) -> Option<String> {
        self.values.get(self.index)?.clone().into_string().ok()
    }

    fn remaining(&mut self) -> Vec<OsString> {
        let remaining = self.values[self.index..].to_vec();
        self.index = self.values.len();
        remaining
    }

    fn finish(&self) -> Result<(), String> {
        if let Some(argument) = self.values.get(self.index) {
            Err(format!("unexpected argument {:?}", argument))
        } else {
            Ok(())
        }
    }
}

fn print_usage() {
    println!(
        "SLIM compiler {}\n\n\
usage:\n  slimc [--message-format=human|json] check <source>\n  \
slimc [--message-format=human|json] emit-c <source> -o <file>\n  \
slimc [--message-format=human|json] build <source> [-o <binary>]\n  \
slimc [--message-format=human|json] run <source> [-- arguments...]\n  \
slimc fmt <source> [--check]\n  slimc runtime <directory>\n  slimc builtins\n",
        crate::VERSION
    );
}

fn print_builtins() {
    const BUILTINS: &[Builtin] = &[
        Builtin::I64Add,
        Builtin::I64Sub,
        Builtin::I64Mul,
        Builtin::I64Div,
        Builtin::I64Rem,
        Builtin::I64Eq,
        Builtin::I64Lt,
        Builtin::I64Le,
        Builtin::I64Gt,
        Builtin::I64Ge,
        Builtin::BoolNot,
        Builtin::BoolAnd,
        Builtin::BoolOr,
        Builtin::U8ToI64,
        Builtin::I64ToU8,
        Builtin::BytesLen,
        Builtin::BytesGet,
        Builtin::IoPrintI64,
        Builtin::IoPrintBytes,
        Builtin::IoPrintln,
        Builtin::IoReadFile,
        Builtin::VecNew,
        Builtin::VecLen,
        Builtin::VecGet,
        Builtin::VecPush,
        Builtin::VecSet,
        Builtin::ArenaNew,
        Builtin::ArenaAdd,
        Builtin::ArenaGet,
    ];
    for builtin in BUILTINS {
        println!("{}", builtin.name());
    }
}
