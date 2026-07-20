use crate::sexpr::{SExpr, SExprKind};

pub fn format_forms(forms: &[SExpr]) -> String {
    let mut output = String::new();
    for (index, form) in forms.iter().enumerate() {
        if index > 0 {
            output.push('\n');
        }
        format_form(form, &mut output);
        output.push('\n');
    }
    output
}

fn format_form(form: &SExpr, output: &mut String) {
    match &form.kind {
        SExprKind::Atom(value) => output.push_str(value),
        SExprKind::Bytes(value) => format_bytes(value, output),
        SExprKind::List(elements) => {
            output.push('(');
            for (index, element) in elements.iter().enumerate() {
                if index > 0 {
                    output.push(' ');
                }
                format_form(element, output);
            }
            output.push(')');
        }
        SExprKind::Error => output.push_str("<error>"),
    }
}

fn format_bytes(value: &[u8], output: &mut String) {
    output.push('"');
    for byte in value {
        match byte {
            b'\\' => output.push_str("\\\\"),
            b'"' => output.push_str("\\\""),
            b'\n' => output.push_str("\\n"),
            b'\r' => output.push_str("\\r"),
            b'\t' => output.push_str("\\t"),
            0x20..=0x7e => output.push(char::from(*byte)),
            _ => output.push_str(&format!("\\x{byte:02x}")),
        }
    }
    output.push('"');
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{lexer, sexpr};

    #[test]
    fn canonical_format_is_idempotent() {
        let source = " (module  x\n(fn main () I64 (effects) 0)) ; end\n";
        let (tokens, errors) = lexer::lex(source);
        assert!(errors.is_empty());
        let (forms, errors) = sexpr::parse(&tokens, source.len());
        assert!(errors.is_empty());
        let first = format_forms(&forms);
        let (tokens, errors) = lexer::lex(&first);
        assert!(errors.is_empty());
        let (forms, errors) = sexpr::parse(&tokens, first.len());
        assert!(errors.is_empty());
        assert_eq!(first, format_forms(&forms));
    }

    #[test]
    fn arbitrary_bytes_round_trip() {
        let source = "\"\\x00\\xff\"";
        let (tokens, errors) = lexer::lex(source);
        assert!(errors.is_empty());
        let (forms, errors) = sexpr::parse(&tokens, source.len());
        assert!(errors.is_empty());
        let formatted = format_forms(&forms);
        assert_eq!(formatted, "\"\\x00\\xff\"\n");
    }
}
