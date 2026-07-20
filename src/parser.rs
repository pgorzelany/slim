use crate::ast::*;
use crate::diagnostic::Diagnostic;
use crate::sexpr::{SExpr, SExprKind};
use crate::span::Span;

pub fn lower(forms: &[SExpr]) -> (Option<Program>, Vec<Diagnostic>) {
    let mut lowerer = Lowerer {
        diagnostics: Vec::new(),
    };
    if forms.is_empty() {
        lowerer.diagnostics.push(Diagnostic::error(
            "E0201",
            "expected one module form",
            Span::default(),
        ));
        return (None, lowerer.diagnostics);
    }
    if forms.len() > 1 {
        for extra in &forms[1..] {
            lowerer.diagnostics.push(
                Diagnostic::error("E0202", "source may contain only one module", extra.span)
                    .note("place every declaration inside the module form"),
            );
        }
    }
    let program = lowerer.program(&forms[0]);
    (program, lowerer.diagnostics)
}

struct Lowerer {
    diagnostics: Vec<Diagnostic>,
}

impl Lowerer {
    fn program(&mut self, form: &SExpr) -> Option<Program> {
        let elements = self.form_named(form, "module", 2)?;
        let name = self.identifier(&elements[1], "module name")?;
        let mut items = Vec::new();
        for item in &elements[2..] {
            if let Some(item) = self.item(item) {
                items.push(item);
            }
        }
        Some(Program {
            name,
            items,
            span: form.span,
        })
    }

    fn item(&mut self, form: &SExpr) -> Option<Item> {
        let Some(elements) = self.list(form) else {
            self.diagnostics.push(Diagnostic::error(
                "E0203",
                "module items must be forms",
                form.span,
            ));
            return None;
        };
        let Some(head) = elements.first().and_then(atom) else {
            self.diagnostics.push(Diagnostic::error(
                "E0204",
                "module item requires a name",
                form.span,
            ));
            return None;
        };
        match head {
            "fn" => self.function(form, elements).map(Item::Function),
            "record" => self.record(form, elements).map(Item::Record),
            "variant" => self.variant(form, elements).map(Item::Variant),
            _ => {
                self.diagnostics.push(
                    Diagnostic::error(
                        "E0205",
                        format!("unknown module item `{head}`"),
                        elements[0].span,
                    )
                    .note("the Core 0 item forms are fn, record, and variant"),
                );
                None
            }
        }
    }

    fn function(&mut self, form: &SExpr, elements: &[SExpr]) -> Option<Function> {
        if !self.exact_arity(form, elements, 6, "fn") {
            return None;
        }
        let name = self.identifier(&elements[1], "function name")?;
        let params_form = self.list(&elements[2])?;
        let mut params = Vec::new();
        for param in params_form {
            let Some(parts) = self.list(param) else {
                self.diagnostics.push(Diagnostic::error(
                    "E0206",
                    "parameter must be (name Type)",
                    param.span,
                ));
                continue;
            };
            if parts.len() != 2 {
                self.diagnostics.push(Diagnostic::error(
                    "E0206",
                    "parameter must contain exactly a name and type",
                    param.span,
                ));
                continue;
            }
            let Some(param_name) = self.identifier(&parts[0], "parameter name") else {
                continue;
            };
            let Some(ty) = self.ty(&parts[1]) else {
                continue;
            };
            params.push(Param {
                name: param_name,
                ty,
                span: param.span,
            });
        }
        let return_type = self.ty(&elements[3])?;
        let effects = self.effects(&elements[4]);
        let body = self.expr(&elements[5]);
        Some(Function {
            name,
            params,
            return_type,
            effects,
            body,
            span: form.span,
        })
    }

    fn record(&mut self, form: &SExpr, elements: &[SExpr]) -> Option<Record> {
        if !self.exact_arity(form, elements, 3, "record") {
            return None;
        }
        let name = self.identifier(&elements[1], "record name")?;
        let fields_form = self.list(&elements[2])?;
        let mut fields = Vec::new();
        for field in fields_form {
            let Some(parts) = self.list(field) else {
                self.diagnostics.push(Diagnostic::error(
                    "E0207",
                    "record field must be (name Type)",
                    field.span,
                ));
                continue;
            };
            if parts.len() != 2 {
                self.diagnostics.push(Diagnostic::error(
                    "E0207",
                    "record field must contain exactly a name and type",
                    field.span,
                ));
                continue;
            }
            let Some(field_name) = self.identifier(&parts[0], "field name") else {
                continue;
            };
            let Some(ty) = self.ty(&parts[1]) else {
                continue;
            };
            fields.push(Field {
                name: field_name,
                ty,
                span: field.span,
            });
        }
        Some(Record {
            name,
            fields,
            span: form.span,
        })
    }

    fn variant(&mut self, form: &SExpr, elements: &[SExpr]) -> Option<Variant> {
        if !self.exact_arity(form, elements, 3, "variant") {
            return None;
        }
        let name = self.identifier(&elements[1], "variant name")?;
        let cases_form = self.list(&elements[2])?;
        let mut cases = Vec::new();
        for case in cases_form {
            let Some(parts) = self.list(case) else {
                self.diagnostics.push(Diagnostic::error(
                    "E0208",
                    "variant case must be (Name Type*)",
                    case.span,
                ));
                continue;
            };
            if parts.is_empty() {
                self.diagnostics.push(Diagnostic::error(
                    "E0208",
                    "variant case requires a name",
                    case.span,
                ));
                continue;
            }
            let Some(case_name) = self.identifier(&parts[0], "case name") else {
                continue;
            };
            let mut payload = Vec::new();
            for ty in &parts[1..] {
                if let Some(ty) = self.ty(ty) {
                    payload.push(ty);
                }
            }
            cases.push(VariantCase {
                name: case_name,
                payload,
                span: case.span,
            });
        }
        Some(Variant {
            name,
            cases,
            span: form.span,
        })
    }

    fn effects(&mut self, form: &SExpr) -> Vec<Effect> {
        let Some(elements) = self.list(form) else {
            self.diagnostics.push(Diagnostic::error(
                "E0209",
                "effects must be written as (effects effect*)",
                form.span,
            ));
            return Vec::new();
        };
        if elements.first().and_then(atom) != Some("effects") {
            self.diagnostics.push(Diagnostic::error(
                "E0209",
                "effect list must start with `effects`",
                form.span,
            ));
            return Vec::new();
        }
        let mut effects = Vec::new();
        for effect in &elements[1..] {
            let parsed = match atom(effect) {
                Some("alloc") => Some(Effect::Alloc),
                Some("io") => Some(Effect::Io),
                Some("partial") => Some(Effect::Partial),
                Some(name) => {
                    self.diagnostics.push(Diagnostic::error(
                        "E0210",
                        format!("unknown effect `{name}`"),
                        effect.span,
                    ));
                    None
                }
                None => {
                    self.diagnostics.push(Diagnostic::error(
                        "E0210",
                        "effect must be a name",
                        effect.span,
                    ));
                    None
                }
            };
            if let Some(parsed) = parsed {
                if effects.contains(&parsed) {
                    self.diagnostics.push(Diagnostic::error(
                        "E0211",
                        format!("duplicate effect `{}`", parsed.name()),
                        effect.span,
                    ));
                } else {
                    effects.push(parsed);
                }
            }
        }
        effects.sort();
        effects
    }

    fn ty(&mut self, form: &SExpr) -> Option<Type> {
        match &form.kind {
            SExprKind::Atom(name) => Some(match name.as_str() {
                "Unit" => Type::Unit,
                "Bool" => Type::Bool,
                "U8" => Type::U8,
                "I64" => Type::I64,
                "Bytes" => Type::Bytes,
                _ if valid_identifier(name) => Type::Named(name.clone()),
                _ => {
                    self.diagnostics.push(Diagnostic::error(
                        "E0212",
                        "invalid type name",
                        form.span,
                    ));
                    Type::Error
                }
            }),
            SExprKind::List(elements) if elements.len() == 2 => {
                let constructor = atom(&elements[0]);
                let inner = self.ty(&elements[1])?;
                match constructor {
                    Some("Vec") => Some(Type::Vec(Box::new(inner))),
                    Some("Arena") => Some(Type::Arena(Box::new(inner))),
                    Some("Id") => Some(Type::Id(Box::new(inner))),
                    Some(name) => {
                        self.diagnostics.push(Diagnostic::error(
                            "E0213",
                            format!("unknown type constructor `{name}`"),
                            elements[0].span,
                        ));
                        Some(Type::Error)
                    }
                    None => {
                        self.diagnostics.push(Diagnostic::error(
                            "E0213",
                            "type constructor must be a name",
                            elements[0].span,
                        ));
                        Some(Type::Error)
                    }
                }
            }
            _ => {
                self.diagnostics.push(Diagnostic::error(
                    "E0214",
                    "type must be a name or one-argument type form",
                    form.span,
                ));
                Some(Type::Error)
            }
        }
    }

    fn expr(&mut self, form: &SExpr) -> Expr {
        let kind = match &form.kind {
            SExprKind::Bytes(value) => ExprKind::Bytes(value.clone()),
            SExprKind::Atom(value) if value == "unit" => ExprKind::Unit,
            SExprKind::Atom(value) if value == "true" => ExprKind::Bool(true),
            SExprKind::Atom(value) if value == "false" => ExprKind::Bool(false),
            SExprKind::Atom(value) => match value.parse::<i64>() {
                Ok(value) => ExprKind::I64(value),
                Err(_) if valid_identifier(value) => ExprKind::Name(value.clone()),
                Err(_) => {
                    self.diagnostics.push(Diagnostic::error(
                        "E0215",
                        "invalid expression atom",
                        form.span,
                    ));
                    ExprKind::Error
                }
            },
            SExprKind::List(elements) => self.list_expr(form, elements),
            SExprKind::Error => ExprKind::Error,
        };
        Expr {
            kind,
            span: form.span,
            ty: Type::Error,
        }
    }

    fn list_expr(&mut self, form: &SExpr, elements: &[SExpr]) -> ExprKind {
        let Some(head) = elements.first().and_then(atom) else {
            self.diagnostics.push(Diagnostic::error(
                "E0216",
                "expression form requires an operation name",
                form.span,
            ));
            return ExprKind::Error;
        };
        match head {
            "let" => {
                if !self.exact_arity(form, elements, 5, "let") {
                    return ExprKind::Error;
                }
                let Some(name) = self.identifier(&elements[1], "binding name") else {
                    return ExprKind::Error;
                };
                let Some(declared_type) = self.ty(&elements[2]) else {
                    return ExprKind::Error;
                };
                ExprKind::Let {
                    name,
                    declared_type,
                    value: Box::new(self.expr(&elements[3])),
                    body: Box::new(self.expr(&elements[4])),
                }
            }
            "call" => {
                if elements.len() < 2 {
                    self.diagnostics.push(Diagnostic::error(
                        "E0217",
                        "call requires a function name",
                        form.span,
                    ));
                    return ExprKind::Error;
                }
                let Some(function) = self.identifier(&elements[1], "called function") else {
                    return ExprKind::Error;
                };
                ExprKind::Call {
                    function,
                    arguments: elements[2..].iter().map(|arg| self.expr(arg)).collect(),
                }
            }
            "recur" => ExprKind::Recur(elements[1..].iter().map(|arg| self.expr(arg)).collect()),
            "set" => {
                if !self.exact_arity(form, elements, 4, "set") {
                    return ExprKind::Error;
                }
                let Some(name) = self.identifier(&elements[1], "mutated binding") else {
                    return ExprKind::Error;
                };
                ExprKind::Set {
                    name,
                    value: Box::new(self.expr(&elements[2])),
                    body: Box::new(self.expr(&elements[3])),
                }
            }
            "make" => {
                if elements.len() < 2 {
                    self.diagnostics.push(Diagnostic::error(
                        "E0218",
                        "make requires a record type",
                        form.span,
                    ));
                    return ExprKind::Error;
                }
                let Some(record) = self.identifier(&elements[1], "record type") else {
                    return ExprKind::Error;
                };
                let mut fields = Vec::new();
                for field in &elements[2..] {
                    let Some(parts) = self.list(field) else {
                        self.diagnostics.push(Diagnostic::error(
                            "E0219",
                            "record initializer must be (field expression)",
                            field.span,
                        ));
                        continue;
                    };
                    if parts.len() != 2 {
                        self.diagnostics.push(Diagnostic::error(
                            "E0219",
                            "record initializer requires exactly a field and expression",
                            field.span,
                        ));
                        continue;
                    }
                    let Some(name) = self.identifier(&parts[0], "initialized field") else {
                        continue;
                    };
                    fields.push((name, self.expr(&parts[1]), field.span));
                }
                ExprKind::Make { record, fields }
            }
            "get" => {
                if !self.exact_arity(form, elements, 3, "get") {
                    return ExprKind::Error;
                }
                let Some(field) = self.identifier(&elements[2], "projected field") else {
                    return ExprKind::Error;
                };
                ExprKind::Get {
                    value: Box::new(self.expr(&elements[1])),
                    field,
                }
            }
            "case" => {
                if elements.len() < 3 {
                    self.diagnostics.push(Diagnostic::error(
                        "E0220",
                        "case requires a variant type and case name",
                        form.span,
                    ));
                    return ExprKind::Error;
                }
                let Some(variant) = self.identifier(&elements[1], "variant type") else {
                    return ExprKind::Error;
                };
                let Some(case) = self.identifier(&elements[2], "variant case") else {
                    return ExprKind::Error;
                };
                ExprKind::Case {
                    variant,
                    case,
                    payload: elements[3..]
                        .iter()
                        .map(|element| self.expr(element))
                        .collect(),
                }
            }
            "match" => self.match_expr(form, elements),
            _ => {
                self.diagnostics.push(
                    Diagnostic::error(
                        "E0221",
                        format!("unknown expression form `{head}`"),
                        elements[0].span,
                    )
                    .note("ordinary function application uses (call name argument*)"),
                );
                ExprKind::Error
            }
        }
    }

    fn match_expr(&mut self, form: &SExpr, elements: &[SExpr]) -> ExprKind {
        if elements.len() < 3 {
            self.diagnostics.push(Diagnostic::error(
                "E0222",
                "match requires a value and at least one arm",
                form.span,
            ));
            return ExprKind::Error;
        }
        let value = Box::new(self.expr(&elements[1]));
        let mut arms = Vec::new();
        for arm in &elements[2..] {
            let Some(parts) = self.list(arm) else {
                self.diagnostics.push(Diagnostic::error(
                    "E0223",
                    "match arm must be a form",
                    arm.span,
                ));
                continue;
            };
            let pattern = if parts.len() == 2 {
                match atom(&parts[0]) {
                    Some("true") => Some(Pattern::Bool(true)),
                    Some("false") => Some(Pattern::Bool(false)),
                    Some(case) => Some(Pattern::Variant {
                        case: case.to_owned(),
                        bindings: Vec::new(),
                    }),
                    None => None,
                }
            } else if parts.len() == 3 {
                let case = self.identifier(&parts[0], "matched case");
                let bindings = self.list(&parts[1]).map(|bindings| {
                    bindings
                        .iter()
                        .filter_map(|binding| self.identifier(binding, "payload binding"))
                        .collect::<Vec<_>>()
                });
                match (case, bindings) {
                    (Some(case), Some(bindings)) => Some(Pattern::Variant { case, bindings }),
                    _ => None,
                }
            } else {
                self.diagnostics.push(Diagnostic::error(
                    "E0224",
                    "match arm must be (pattern body) or (Case (bindings*) body)",
                    arm.span,
                ));
                None
            };
            if let Some(pattern) = pattern {
                let body_index = parts.len() - 1;
                arms.push(MatchArm {
                    pattern,
                    body: self.expr(&parts[body_index]),
                    span: arm.span,
                });
            }
        }
        ExprKind::Match { value, arms }
    }

    fn form_named<'a>(
        &mut self,
        form: &'a SExpr,
        expected: &str,
        minimum: usize,
    ) -> Option<&'a [SExpr]> {
        let elements = self.list(form)?;
        if elements.len() < minimum || elements.first().and_then(atom) != Some(expected) {
            self.diagnostics.push(Diagnostic::error(
                "E0225",
                format!("expected ({expected} ... )"),
                form.span,
            ));
            None
        } else {
            Some(elements)
        }
    }

    fn list<'a>(&mut self, form: &'a SExpr) -> Option<&'a [SExpr]> {
        match &form.kind {
            SExprKind::List(elements) => Some(elements),
            _ => None,
        }
    }

    fn exact_arity(
        &mut self,
        form: &SExpr,
        elements: &[SExpr],
        expected: usize,
        name: &str,
    ) -> bool {
        if elements.len() == expected {
            true
        } else {
            self.diagnostics.push(Diagnostic::error(
                "E0226",
                format!(
                    "{name} expects {} argument(s), found {}",
                    expected - 1,
                    elements.len().saturating_sub(1)
                ),
                form.span,
            ));
            false
        }
    }

    fn identifier(&mut self, form: &SExpr, role: &str) -> Option<String> {
        let Some(value) = atom(form) else {
            self.diagnostics.push(Diagnostic::error(
                "E0227",
                format!("{role} must be an identifier"),
                form.span,
            ));
            return None;
        };
        if !valid_identifier(value) {
            self.diagnostics.push(Diagnostic::error(
                "E0228",
                format!("invalid {role} `{value}`"),
                form.span,
            ));
            None
        } else {
            Some(value.to_owned())
        }
    }
}

fn atom(form: &SExpr) -> Option<&str> {
    match &form.kind {
        SExprKind::Atom(value) => Some(value),
        _ => None,
    }
}

fn valid_identifier(value: &str) -> bool {
    let mut bytes = value.bytes();
    let Some(first) = bytes.next() else {
        return false;
    };
    (first.is_ascii_alphabetic() || first == b'_')
        && bytes.all(|byte| {
            byte.is_ascii_alphanumeric() || matches!(byte, b'_' | b'-' | b'.' | b'/' | b'?' | b'!')
        })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::lex;
    use crate::sexpr;

    fn parse_source(source: &str) -> (Option<Program>, Vec<Diagnostic>) {
        let (tokens, mut diagnostics) = lex(source);
        let (forms, parse_diagnostics) = sexpr::parse(&tokens, source.len());
        diagnostics.extend(parse_diagnostics);
        let (program, lower_diagnostics) = lower(&forms);
        diagnostics.extend(lower_diagnostics);
        (program, diagnostics)
    }

    #[test]
    fn lowers_minimal_program() {
        let (program, diagnostics) =
            parse_source("(module hello (fn main () I64 (effects) (call i64.add 40 2)))");
        assert!(diagnostics.is_empty(), "{diagnostics:#?}");
        let program = program.unwrap();
        assert_eq!(program.name, "hello");
        assert_eq!(program.items.len(), 1);
    }

    #[test]
    fn reports_unknown_expression_and_continues() {
        let (_, diagnostics) = parse_source("(module bad (fn main () I64 (effects) (plus 1 2)))");
        assert!(
            diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0221")
        );
    }
}
