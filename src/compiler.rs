use crate::ast::Program;
use crate::codegen;
use crate::diagnostic::Diagnostic;
use crate::formatter;
use crate::lexer;
use crate::parser;
use crate::sema::{self, CheckedProgram};
use crate::sexpr;
use crate::span::Source;

#[derive(Debug)]
pub struct Compilation {
    pub source: Source,
    pub checked: Option<CheckedProgram>,
    pub diagnostics: Vec<Diagnostic>,
}

impl Compilation {
    pub fn succeeded(&self) -> bool {
        self.checked.is_some() && self.diagnostics.is_empty()
    }

    pub fn emit_c(&self) -> Option<String> {
        self.checked.as_ref().map(codegen::generate_c)
    }
}

pub fn compile(source: Source) -> Compilation {
    let (program, mut diagnostics) = lower_source(&source);
    let checked = if let Some(program) = program {
        let (checked, check_diagnostics) = sema::check(program);
        diagnostics.extend(check_diagnostics);
        checked
    } else {
        None
    };
    diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
    Compilation {
        source,
        checked,
        diagnostics,
    }
}

pub fn lower_source(source: &Source) -> (Option<Program>, Vec<Diagnostic>) {
    let (tokens, mut diagnostics) = lexer::lex(&source.text);
    let (forms, parse_diagnostics) = sexpr::parse(&tokens, source.text.len());
    diagnostics.extend(parse_diagnostics);
    let (program, lower_diagnostics) = parser::lower(&forms);
    diagnostics.extend(lower_diagnostics);
    diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
    (program, diagnostics)
}

pub fn format_source(source: &Source) -> Result<String, Vec<Diagnostic>> {
    let (tokens, mut diagnostics) = lexer::lex(&source.text);
    let (forms, parse_diagnostics) = sexpr::parse(&tokens, source.text.len());
    diagnostics.extend(parse_diagnostics);
    diagnostics.sort_by_key(|diagnostic| (diagnostic.primary.start, diagnostic.code));
    if diagnostics.is_empty() {
        Ok(formatter::format_forms(&forms))
    } else {
        Err(diagnostics)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    #[test]
    fn returns_multiple_sorted_diagnostics() {
        let source = Source::new(
            PathBuf::from("bad.slim"),
            "(module bad (fn main ((args (Vec Bytes))) I64 (effects) (match true (true missing))))"
                .to_owned(),
        );
        let compilation = compile(source);
        assert!(!compilation.succeeded());
        assert!(compilation.diagnostics.len() >= 2);
        assert!(
            compilation
                .diagnostics
                .windows(2)
                .all(|window| window[0].primary.start <= window[1].primary.start)
        );
    }

    #[test]
    fn malformed_ascii_never_panics() {
        let alphabet: &[u8] = b"()abcXYZ012-; \\\"\n";
        let mut state = 0x5eed_u64;
        for case in 0..2_000 {
            let length = case % 257;
            let mut text = String::with_capacity(length);
            for _ in 0..length {
                state = state.wrapping_mul(6364136223846793005).wrapping_add(1);
                text.push(char::from(alphabet[(state as usize) % alphabet.len()]));
            }
            let result = std::panic::catch_unwind(|| {
                compile(Source::new(PathBuf::from("mutated.slim"), text))
            });
            assert!(result.is_ok(), "compiler panicked for mutation {case}");
        }
    }
}
