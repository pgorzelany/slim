//! SLIM stage-0 compiler library.

pub mod ast;
pub mod bootstrap;
pub mod codegen;
pub mod compiler;
pub mod diagnostic;
pub mod driver;
pub mod formatter;
pub mod incremental;
pub mod lexer;
pub mod parser;
pub mod sema;
pub mod sexpr;
pub mod span;

/// Compiler version reported in diagnostics and generated artifacts.
pub const VERSION: &str = env!("CARGO_PKG_VERSION");
