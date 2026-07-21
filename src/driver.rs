use std::env;
use std::ffi::OsString;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::process::{Command, ExitStatus};
use std::time::{SystemTime, UNIX_EPOCH};

use crate::compiler::{self, Compilation};
use crate::diagnostic::Diagnostic;
use crate::project::{self, ProjectCompilation};
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
            let jobs = parse_jobs(&mut args)?;
            args.finish()?;
            let compilation = compile_path(&path, jobs)?;
            render_input_diagnostics(&compilation, message_format);
            Ok(if compilation.succeeded() { 0 } else { 1 })
        }
        "emit-c" => {
            let source_path = required_path(&mut args, "source file")?;
            let jobs = parse_jobs(&mut args)?;
            let output = parse_output(&mut args)?;
            args.finish()?;
            let compilation = compile_path(&source_path, jobs)?;
            render_input_diagnostics(&compilation, message_format);
            let Some(c) = compilation.emit_c() else {
                return Ok(1);
            };
            write_file(&output, c.as_bytes())?;
            Ok(0)
        }
        "build" => {
            let source_path = required_path(&mut args, "source file")?;
            let jobs = parse_jobs(&mut args)?;
            let output =
                parse_optional_output(&mut args)?.unwrap_or_else(|| default_output(&source_path));
            args.finish()?;
            let compilation = compile_path(&source_path, jobs)?;
            render_input_diagnostics(&compilation, message_format);
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
            let jobs = parse_jobs(&mut args)?;
            let program_arguments = if args.peek_string().as_deref() == Some("--") {
                args.next();
                args.remaining()
            } else {
                args.finish()?;
                Vec::new()
            };
            let compilation = compile_path(&source_path, jobs)?;
            render_input_diagnostics(&compilation, message_format);
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
        "interfaces" => {
            let path = required_path(&mut args, "project manifest")?;
            let jobs = parse_jobs(&mut args)?;
            let output = parse_output(&mut args)?;
            args.finish()?;
            let source = read_source(&path)?;
            if !project::is_manifest_source(&source) {
                return Err("interfaces requires an explicit (project ...) manifest".to_owned());
            }
            let compilation = project::compile_cached_with_jobs(source, jobs.unwrap_or(1));
            render_project_diagnostics(&compilation, message_format);
            if !compilation.succeeded() {
                return Ok(1);
            }
            fs::create_dir_all(&output)
                .map_err(|error| format!("cannot create {}: {error}", output.display()))?;
            for (module, artifact) in &compilation.interfaces {
                write_file(
                    &output.join(format!("{module}.sli")),
                    artifact.bytes.as_bytes(),
                )?;
            }
            Ok(0)
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

enum InputCompilation {
    Module(Compilation),
    Project(ProjectCompilation),
}

impl InputCompilation {
    fn succeeded(&self) -> bool {
        match self {
            Self::Module(compilation) => compilation.succeeded(),
            Self::Project(compilation) => compilation.succeeded(),
        }
    }

    fn emit_c(&self) -> Option<String> {
        match self {
            Self::Module(compilation) => compilation.emit_c(),
            Self::Project(compilation) => compilation.emit_c().map(str::to_owned),
        }
    }
}

fn compile_path(path: &Path, jobs: Option<usize>) -> Result<InputCompilation, String> {
    let source = read_source(path)?;
    if project::is_manifest_source(&source) {
        Ok(InputCompilation::Project(
            project::compile_cached_with_jobs(source, jobs.unwrap_or(1)),
        ))
    } else {
        if jobs.is_some() {
            return Err(
                "--jobs is available only for an explicit (project ...) manifest".to_owned(),
            );
        }
        Ok(InputCompilation::Module(compiler::compile(source)))
    }
}

fn read_source(path: &Path) -> Result<Source, String> {
    let text = fs::read_to_string(path)
        .map_err(|error| format!("cannot read {} as UTF-8: {error}", path.display()))?;
    Ok(Source::new(path, text))
}

fn render_input_diagnostics(compilation: &InputCompilation, format: MessageFormat) {
    match compilation {
        InputCompilation::Module(compilation) => {
            render_diagnostic_list(&compilation.source, &compilation.diagnostics, format)
        }
        InputCompilation::Project(compilation) => render_project_diagnostics(compilation, format),
    }
}

fn render_project_diagnostics(compilation: &ProjectCompilation, format: MessageFormat) {
    for diagnostic in &compilation.diagnostics {
        match format {
            MessageFormat::Human => eprint!("{}", diagnostic.render_human()),
            MessageFormat::Json => eprintln!("{}", diagnostic.render_json()),
        }
    }
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

fn parse_jobs(args: &mut Arguments) -> Result<Option<usize>, String> {
    if args.peek_string().as_deref() != Some("--jobs") {
        return Ok(None);
    }
    args.next();
    let value = args
        .next_string()
        .ok_or_else(|| "missing required positive integer after `--jobs`".to_owned())?;
    let jobs = value
        .parse::<usize>()
        .ok()
        .filter(|jobs| *jobs > 0)
        .ok_or_else(|| format!("invalid job count `{value}`; expected a positive integer"))?;
    Ok(Some(jobs))
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
        if let Some(parent) = output.parent() {
            fs::create_dir_all(parent)
                .map_err(|error| format!("cannot create {}: {error}", parent.display()))?;
        }
        let compiler = env::var_os("CC").unwrap_or_else(|| OsString::from("clang"));
        Command::new(&compiler)
            .arg("-std=c11")
            .arg("-O3")
            .arg("-flto")
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
usage:\n  slimc [--message-format=human|json] check <source> [--jobs N]\n  \
slimc [--message-format=human|json] emit-c <source> [--jobs N] -o <file>\n  \
slimc [--message-format=human|json] build <source> [--jobs N] [-o <binary>]\n  \
slimc [--message-format=human|json] run <source> [--jobs N] [-- arguments...]\n  \
slimc fmt <source> [--check]\n  slimc interfaces <project> -o <directory>\n  \
slimc runtime <directory>\n  slimc builtins\n",
        crate::VERSION
    );
}

fn print_builtins() {
    for builtin in Builtin::all() {
        println!("{}", builtin.name());
    }
}
