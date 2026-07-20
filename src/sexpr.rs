use crate::diagnostic::Diagnostic;
use crate::lexer::{Token, TokenKind};
use crate::span::Span;

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum SExprKind {
    Atom(String),
    Bytes(Vec<u8>),
    List(Vec<SExpr>),
    Error,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct SExpr {
    pub kind: SExprKind,
    pub span: Span,
}

pub fn parse(tokens: &[Token], source_len: usize) -> (Vec<SExpr>, Vec<Diagnostic>) {
    let mut parser = Parser {
        tokens,
        index: 0,
        source_len,
        diagnostics: Vec::new(),
    };
    let mut forms = Vec::new();
    while parser.index < tokens.len() {
        forms.push(parser.form());
    }
    (forms, parser.diagnostics)
}

struct Parser<'a> {
    tokens: &'a [Token],
    index: usize,
    source_len: usize,
    diagnostics: Vec<Diagnostic>,
}

impl Parser<'_> {
    fn form(&mut self) -> SExpr {
        let Some(token) = self.tokens.get(self.index) else {
            return SExpr {
                kind: SExprKind::Error,
                span: Span::new(self.source_len, self.source_len),
            };
        };
        self.index += 1;
        match &token.kind {
            TokenKind::Atom(value) => SExpr {
                kind: SExprKind::Atom(value.clone()),
                span: token.span,
            },
            TokenKind::Bytes(value) => SExpr {
                kind: SExprKind::Bytes(value.clone()),
                span: token.span,
            },
            TokenKind::RightParen => {
                self.diagnostics.push(
                    Diagnostic::error("E0101", "unmatched closing parenthesis", token.span).fix(
                        token.span,
                        "",
                        "remove this parenthesis",
                    ),
                );
                SExpr {
                    kind: SExprKind::Error,
                    span: token.span,
                }
            }
            TokenKind::LeftParen => self.list(token.span),
        }
    }

    fn list(&mut self, opening: Span) -> SExpr {
        let mut elements = Vec::new();
        loop {
            let Some(token) = self.tokens.get(self.index) else {
                self.diagnostics.push(
                    Diagnostic::error(
                        "E0102",
                        "unterminated form",
                        Span::new(opening.start, self.source_len),
                    )
                    .label(opening, "form starts here")
                    .fix(
                        Span::new(self.source_len, self.source_len),
                        ")",
                        "close this form",
                    ),
                );
                return SExpr {
                    kind: SExprKind::List(elements),
                    span: Span::new(opening.start, self.source_len),
                };
            };
            if token.kind == TokenKind::RightParen {
                self.index += 1;
                return SExpr {
                    kind: SExprKind::List(elements),
                    span: opening.merge(token.span),
                };
            }
            elements.push(self.form());
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::lex;

    #[test]
    fn recovers_unterminated_list() {
        let source = "(a (b)";
        let (tokens, lex_errors) = lex(source);
        assert!(lex_errors.is_empty());
        let (forms, errors) = parse(&tokens, source.len());
        assert_eq!(forms.len(), 1);
        assert_eq!(errors[0].code, "E0102");
    }
}
