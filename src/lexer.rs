use crate::diagnostic::Diagnostic;
use crate::span::Span;

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum TokenKind {
    LeftParen,
    RightParen,
    Atom(String),
    Bytes(Vec<u8>),
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub span: Span,
}

pub fn lex(source: &str) -> (Vec<Token>, Vec<Diagnostic>) {
    let bytes = source.as_bytes();
    let mut tokens = Vec::new();
    let mut diagnostics = Vec::new();
    let mut index = 0;

    while index < bytes.len() {
        match bytes[index] {
            byte if byte.is_ascii_whitespace() => index += 1,
            b';' => {
                while index < bytes.len() && bytes[index] != b'\n' {
                    index += 1;
                }
            }
            b'(' => {
                tokens.push(Token {
                    kind: TokenKind::LeftParen,
                    span: Span::new(index, index + 1),
                });
                index += 1;
            }
            b')' => {
                tokens.push(Token {
                    kind: TokenKind::RightParen,
                    span: Span::new(index, index + 1),
                });
                index += 1;
            }
            b'"' => {
                let start = index;
                index += 1;
                let mut value = Vec::new();
                let mut terminated = false;
                while index < bytes.len() {
                    match bytes[index] {
                        b'"' => {
                            index += 1;
                            terminated = true;
                            break;
                        }
                        b'\\' => {
                            let escape_start = index;
                            index += 1;
                            if index >= bytes.len() {
                                break;
                            }
                            if bytes[index] == b'x' {
                                let high = bytes.get(index + 1).copied().and_then(hex_value);
                                let low = bytes.get(index + 2).copied().and_then(hex_value);
                                if let (Some(high), Some(low)) = (high, low) {
                                    value.push((high << 4) | low);
                                    index += 3;
                                    continue;
                                }
                                diagnostics.push(
                                    Diagnostic::error(
                                        "E0002",
                                        "byte escape must contain two hexadecimal digits",
                                        Span::new(escape_start, (index + 3).min(bytes.len())),
                                    )
                                    .note("write a byte escape as \\xNN"),
                                );
                                index += 1;
                                continue;
                            }
                            let decoded = match bytes[index] {
                                b'\\' => Some(b'\\'),
                                b'"' => Some(b'"'),
                                b'n' => Some(b'\n'),
                                b'r' => Some(b'\r'),
                                b't' => Some(b'\t'),
                                _ => None,
                            };
                            if let Some(decoded) = decoded {
                                value.push(decoded);
                            } else {
                                diagnostics.push(
                                    Diagnostic::error(
                                        "E0002",
                                        "unknown byte-string escape",
                                        Span::new(escape_start, index + 1),
                                    )
                                    .note("valid escapes are \\\\, \\\", \\n, \\r, \\t, and \\xNN"),
                                );
                            }
                            index += 1;
                        }
                        byte if byte < 0x20 && byte != b'\t' => {
                            diagnostics.push(Diagnostic::error(
                                "E0003",
                                "control byte in string literal",
                                Span::new(index, index + 1),
                            ));
                            value.push(byte);
                            index += 1;
                        }
                        byte => {
                            value.push(byte);
                            index += 1;
                        }
                    }
                }
                if !terminated {
                    diagnostics.push(
                        Diagnostic::error(
                            "E0004",
                            "unterminated byte string",
                            Span::new(start, index),
                        )
                        .fix(
                            Span::new(index, index),
                            "\"",
                            "terminate the string",
                        ),
                    );
                }
                tokens.push(Token {
                    kind: TokenKind::Bytes(value),
                    span: Span::new(start, index),
                });
            }
            _ => {
                let start = index;
                while index < bytes.len()
                    && !bytes[index].is_ascii_whitespace()
                    && !matches!(bytes[index], b'(' | b')' | b';' | b'"')
                {
                    index += 1;
                }
                let atom = &source[start..index];
                if atom.is_empty() {
                    diagnostics.push(Diagnostic::error(
                        "E0001",
                        "invalid source byte",
                        Span::new(index, index + 1),
                    ));
                    index += 1;
                } else {
                    tokens.push(Token {
                        kind: TokenKind::Atom(atom.to_owned()),
                        span: Span::new(start, index),
                    });
                }
            }
        }
    }

    (tokens, diagnostics)
}

fn hex_value(byte: u8) -> Option<u8> {
    match byte {
        b'0'..=b'9' => Some(byte - b'0'),
        b'a'..=b'f' => Some(byte - b'a' + 10),
        b'A'..=b'F' => Some(byte - b'A' + 10),
        _ => None,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn lexes_forms_comments_and_escapes() {
        let (tokens, diagnostics) = lex("(call io.println \"a\\n\") ; rest\n");
        assert!(diagnostics.is_empty());
        assert_eq!(tokens.len(), 5);
        assert_eq!(tokens[0].kind, TokenKind::LeftParen);
        assert_eq!(tokens[3].kind, TokenKind::Bytes(b"a\n".to_vec()));
    }

    #[test]
    fn reports_bad_escape_without_stopping() {
        let (tokens, diagnostics) = lex("\"a\\q\" next");
        assert_eq!(tokens.len(), 2);
        assert_eq!(diagnostics[0].code, "E0002");
    }

    #[test]
    fn lexes_numeric_byte_escape() {
        let (tokens, diagnostics) = lex("\"A\\x00\\xff\"");
        assert!(diagnostics.is_empty());
        assert_eq!(tokens[0].kind, TokenKind::Bytes(vec![b'A', 0, 255]));
    }
}
