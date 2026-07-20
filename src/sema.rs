use std::collections::{BTreeMap, BTreeSet};

use crate::ast::*;
use crate::diagnostic::Diagnostic;
use crate::span::Span;

#[derive(Clone, Debug)]
pub struct CheckedProgram {
    pub program: Program,
    pub records: BTreeMap<String, Record>,
    pub variants: BTreeMap<String, Variant>,
    pub functions: BTreeMap<String, FunctionSignature>,
}

#[derive(Clone, Debug)]
pub struct FunctionSignature {
    pub params: Vec<Type>,
    pub return_type: Type,
    pub effects: BTreeSet<Effect>,
    pub span: Span,
}

pub fn check(mut program: Program) -> (Option<CheckedProgram>, Vec<Diagnostic>) {
    let mut checker = Checker::new();
    checker.collect(&program);
    checker.validate_declared_types();
    checker.check_functions(&mut program);
    checker.check_entry(&program);
    if checker
        .diagnostics
        .iter()
        .any(|diagnostic| diagnostic.severity == crate::diagnostic::Severity::Error)
    {
        (None, checker.diagnostics)
    } else {
        (
            Some(CheckedProgram {
                program,
                records: checker.records,
                variants: checker.variants,
                functions: checker.functions,
            }),
            checker.diagnostics,
        )
    }
}

struct Checker {
    diagnostics: Vec<Diagnostic>,
    records: BTreeMap<String, Record>,
    variants: BTreeMap<String, Variant>,
    functions: BTreeMap<String, FunctionSignature>,
}

impl Checker {
    fn new() -> Self {
        Self {
            diagnostics: Vec::new(),
            records: BTreeMap::new(),
            variants: BTreeMap::new(),
            functions: BTreeMap::new(),
        }
    }

    fn collect(&mut self, program: &Program) {
        let mut names = BTreeMap::<String, Span>::new();
        for item in &program.items {
            let (name, span) = match item {
                Item::Function(function) => (&function.name, function.span),
                Item::Record(record) => (&record.name, record.span),
                Item::Variant(variant) => (&variant.name, variant.span),
            };
            if let Some(previous) = names.insert(name.clone(), span) {
                self.diagnostics.push(
                    Diagnostic::error("E0301", format!("duplicate declaration `{name}`"), span)
                        .label(previous, "first declaration is here"),
                );
                continue;
            }
            match item {
                Item::Function(function) => {
                    self.functions.insert(
                        function.name.clone(),
                        FunctionSignature {
                            params: function
                                .params
                                .iter()
                                .map(|param| param.ty.clone())
                                .collect(),
                            return_type: function.return_type.clone(),
                            effects: function.effects.iter().copied().collect(),
                            span: function.span,
                        },
                    );
                }
                Item::Record(record) => {
                    self.records.insert(record.name.clone(), record.clone());
                }
                Item::Variant(variant) => {
                    self.variants.insert(variant.name.clone(), variant.clone());
                }
            }
        }
    }

    fn validate_declared_types(&mut self) {
        let records: Vec<_> = self.records.values().cloned().collect();
        for record in records {
            let mut fields = BTreeMap::<String, Span>::new();
            for field in &record.fields {
                if let Some(previous) = fields.insert(field.name.clone(), field.span) {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0302",
                            format!("duplicate field `{}`", field.name),
                            field.span,
                        )
                        .label(previous, "first field is here"),
                    );
                }
                self.validate_type(&field.ty, field.span);
            }
        }
        let variants: Vec<_> = self.variants.values().cloned().collect();
        for variant in variants {
            let mut cases = BTreeMap::<String, Span>::new();
            for case in &variant.cases {
                if let Some(previous) = cases.insert(case.name.clone(), case.span) {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0303",
                            format!("duplicate case `{}`", case.name),
                            case.span,
                        )
                        .label(previous, "first case is here"),
                    );
                }
                for ty in &case.payload {
                    self.validate_type(ty, case.span);
                }
            }
        }
        let functions: Vec<_> = self
            .functions
            .iter()
            .map(|(name, signature)| (name.clone(), signature.clone()))
            .collect();
        for (name, signature) in functions {
            for ty in &signature.params {
                self.validate_type(ty, signature.span);
            }
            self.validate_type(&signature.return_type, signature.span);
            if is_reserved_function(&name) {
                self.diagnostics.push(Diagnostic::error(
                    "E0304",
                    format!("`{name}` is a reserved built-in function name"),
                    signature.span,
                ));
            }
        }
    }

    fn validate_type(&mut self, ty: &Type, span: Span) {
        match ty {
            Type::Named(name)
                if !self.records.contains_key(name) && !self.variants.contains_key(name) =>
            {
                self.diagnostics.push(Diagnostic::error(
                    "E0305",
                    format!("unknown type `{name}`"),
                    span,
                ));
            }
            Type::Vec(inner) | Type::Arena(inner) | Type::Id(inner) => {
                self.validate_type(inner, span)
            }
            _ => {}
        }
    }

    fn check_functions(&mut self, program: &mut Program) {
        for item in &mut program.items {
            let Item::Function(function) = item else {
                continue;
            };
            let mut env = Environment::default();
            let mut parameter_names = BTreeMap::new();
            for parameter in &function.params {
                if let Some(previous) =
                    parameter_names.insert(parameter.name.clone(), parameter.span)
                {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0306",
                            format!("duplicate parameter `{}`", parameter.name),
                            parameter.span,
                        )
                        .label(previous, "first parameter is here"),
                    );
                }
                env.bindings.push(Binding {
                    name: parameter.name.clone(),
                    ty: parameter.ty.clone(),
                    declared_at: parameter.span,
                    moved_at: None,
                });
            }
            let allowed_effects: BTreeSet<_> = function.effects.iter().copied().collect();
            let context = FunctionContext {
                name: &function.name,
                params: &function.params,
                return_type: &function.return_type,
                effects: &allowed_effects,
            };
            let body_type = self.check_expr(
                &mut function.body,
                &mut env,
                &context,
                Some(&function.return_type),
                Use::Move,
                true,
            );
            self.expect_type(&body_type, &function.return_type, function.body.span);
        }
    }

    fn check_entry(&mut self, program: &Program) {
        let Some(signature) = self.functions.get("main") else {
            self.diagnostics.push(Diagnostic::error(
                "E0307",
                "module requires `main`",
                program.span,
            ));
            return;
        };
        if !signature.params.is_empty() || signature.return_type != Type::I64 {
            self.diagnostics.push(
                Diagnostic::error("E0308", "`main` must have type () -> I64", signature.span)
                    .note("write (fn main () I64 (effects ...) body)"),
            );
        }
    }

    #[allow(clippy::too_many_arguments)]
    fn check_expr(
        &mut self,
        expr: &mut Expr,
        env: &mut Environment,
        context: &FunctionContext<'_>,
        expected: Option<&Type>,
        usage: Use,
        tail: bool,
    ) -> Type {
        let ty = match &mut expr.kind {
            ExprKind::Unit => Type::Unit,
            ExprKind::Bool(_) => Type::Bool,
            ExprKind::I64(_) => Type::I64,
            ExprKind::Bytes(_) => Type::Bytes,
            ExprKind::Name(name) => self.use_name(name, expr.span, env, usage),
            ExprKind::Let {
                name,
                declared_type,
                value,
                body,
            } => {
                let value_type =
                    self.check_expr(value, env, context, Some(declared_type), Use::Move, false);
                self.expect_type(&value_type, declared_type, value.span);
                if let Some(previous) = env
                    .bindings
                    .iter()
                    .rev()
                    .find(|binding| binding.name == *name)
                {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0309",
                            format!("binding `{name}` shadows an existing name"),
                            expr.span,
                        )
                        .label(previous.declared_at, "existing binding is here")
                        .note("Core uses unique local names to keep data flow explicit"),
                    );
                }
                env.bindings.push(Binding {
                    name: name.clone(),
                    ty: declared_type.clone(),
                    declared_at: expr.span,
                    moved_at: None,
                });
                let body_type = self.check_expr(body, env, context, expected, usage, tail);
                env.bindings.pop();
                body_type
            }
            ExprKind::Call {
                function,
                arguments,
            } => self.check_call(function, arguments, expr.span, env, context, expected),
            ExprKind::Recur(arguments) => {
                if !tail {
                    self.diagnostics.push(Diagnostic::error(
                        "E0310",
                        "recur is only valid in tail position",
                        expr.span,
                    ));
                }
                self.require_effect(Effect::Partial, expr.span, context);
                if arguments.len() != context.params.len() {
                    self.diagnostics.push(Diagnostic::error(
                        "E0311",
                        format!(
                            "recur for `{}` expects {} argument(s), found {}",
                            context.name,
                            context.params.len(),
                            arguments.len()
                        ),
                        expr.span,
                    ));
                }
                for (argument, parameter) in arguments.iter_mut().zip(context.params) {
                    let argument_type = self.check_expr(
                        argument,
                        env,
                        context,
                        Some(&parameter.ty),
                        // Recurrence updates all parameters simultaneously.
                        // Borrow roots while evaluating every new argument so
                        // later arguments may still inspect an owned parameter.
                        Use::Borrow,
                        false,
                    );
                    self.expect_type(&argument_type, &parameter.ty, argument.span);
                }
                context.return_type.clone()
            }
            ExprKind::Set { name, value, body } => {
                let binding = env
                    .bindings
                    .iter()
                    .rev()
                    .find(|binding| binding.name == *name)
                    .cloned();
                let Some(binding) = binding else {
                    self.diagnostics.push(Diagnostic::error(
                        "E0312",
                        format!("cannot set unknown binding `{name}`"),
                        expr.span,
                    ));
                    return self.set_expr_type(expr, Type::Error);
                };
                if let Some(moved_at) = binding.moved_at {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0313",
                            format!("cannot set moved binding `{name}`"),
                            expr.span,
                        )
                        .label(moved_at, "value moved here"),
                    );
                }
                let value_type =
                    self.check_expr(value, env, context, Some(&binding.ty), Use::Move, false);
                self.expect_type(&value_type, &binding.ty, value.span);
                if let Some(binding) = env
                    .bindings
                    .iter_mut()
                    .rev()
                    .find(|item| item.name == *name)
                {
                    binding.moved_at = None;
                }
                self.check_expr(body, env, context, expected, usage, tail)
            }
            ExprKind::Make { record, fields } => {
                self.check_make(record, fields, expr.span, env, context)
            }
            ExprKind::Get { value, field } => {
                let value_type = self.check_expr(value, env, context, None, Use::Borrow, false);
                self.check_get(&value_type, field, expr.span)
            }
            ExprKind::Case {
                variant,
                case,
                payload,
            } => self.check_case(variant, case, payload, expr.span, env, context),
            ExprKind::Match { value, arms } => {
                self.check_match(value, arms, expr.span, env, context, expected, usage, tail)
            }
            ExprKind::Error => Type::Error,
        };
        expr.ty = ty.clone();
        ty
    }

    fn set_expr_type(&self, expr: &mut Expr, ty: Type) -> Type {
        expr.ty = ty.clone();
        ty
    }

    fn use_name(&mut self, name: &str, span: Span, env: &mut Environment, usage: Use) -> Type {
        let Some(binding) = env
            .bindings
            .iter_mut()
            .rev()
            .find(|binding| binding.name == name)
        else {
            self.diagnostics.push(Diagnostic::error(
                "E0314",
                format!("unknown binding `{name}`"),
                span,
            ));
            return Type::Error;
        };
        if let Some(moved_at) = binding.moved_at {
            self.diagnostics.push(
                Diagnostic::error("E0315", format!("use of moved value `{name}`"), span)
                    .label(moved_at, "value moved here")
                    .label(binding.declared_at, "value declared here"),
            );
            return Type::Error;
        }
        let ty = binding.ty.clone();
        if usage == Use::Move && !self.is_copy_type(&ty, &mut BTreeSet::new()) {
            binding.moved_at = Some(span);
        }
        ty
    }

    fn check_call(
        &mut self,
        function: &str,
        arguments: &mut [Expr],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
        expected: Option<&Type>,
    ) -> Type {
        if let Some(builtin) = Builtin::from_name(function) {
            return self.check_builtin(builtin, arguments, span, env, context, expected);
        }
        let Some(signature) = self.functions.get(function).cloned() else {
            self.diagnostics.push(Diagnostic::error(
                "E0316",
                format!("unknown function `{function}`"),
                span,
            ));
            for argument in arguments {
                self.check_expr(argument, env, context, None, Use::Move, false);
            }
            return Type::Error;
        };
        if arguments.len() != signature.params.len() {
            self.diagnostics.push(
                Diagnostic::error(
                    "E0317",
                    format!(
                        "`{function}` expects {} argument(s), found {}",
                        signature.params.len(),
                        arguments.len()
                    ),
                    span,
                )
                .label(signature.span, "function declared here"),
            );
        }
        for (argument, parameter_type) in arguments.iter_mut().zip(&signature.params) {
            let argument_type = self.check_expr(
                argument,
                env,
                context,
                Some(parameter_type),
                Use::Move,
                false,
            );
            self.expect_type(&argument_type, parameter_type, argument.span);
        }
        for effect in signature.effects {
            self.require_effect(effect, span, context);
        }
        signature.return_type
    }

    fn check_builtin(
        &mut self,
        builtin: Builtin,
        arguments: &mut [Expr],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
        expected: Option<&Type>,
    ) -> Type {
        for effect in builtin.effects() {
            self.require_effect(*effect, span, context);
        }
        match builtin {
            Builtin::I64Add
            | Builtin::I64Sub
            | Builtin::I64Mul
            | Builtin::I64Div
            | Builtin::I64Rem => {
                self.check_builtin_args(arguments, &[Type::I64, Type::I64], env, context, span);
                Type::I64
            }
            Builtin::I64Eq | Builtin::I64Lt | Builtin::I64Le | Builtin::I64Gt | Builtin::I64Ge => {
                self.check_builtin_args(arguments, &[Type::I64, Type::I64], env, context, span);
                Type::Bool
            }
            Builtin::BoolNot => {
                self.check_builtin_args(arguments, &[Type::Bool], env, context, span);
                Type::Bool
            }
            Builtin::BoolAnd | Builtin::BoolOr => {
                self.check_builtin_args(arguments, &[Type::Bool, Type::Bool], env, context, span);
                Type::Bool
            }
            Builtin::U8ToI64 => {
                self.check_builtin_args(arguments, &[Type::U8], env, context, span);
                Type::I64
            }
            Builtin::I64ToU8 => {
                self.check_builtin_args(arguments, &[Type::I64], env, context, span);
                Type::U8
            }
            Builtin::BytesLen => {
                self.check_borrowing_args(arguments, &[Type::Bytes], env, context, span);
                Type::I64
            }
            Builtin::BytesGet => {
                self.check_mixed_args(
                    arguments,
                    &[(Type::Bytes, Use::Borrow), (Type::I64, Use::Move)],
                    env,
                    context,
                    span,
                );
                Type::U8
            }
            Builtin::IoPrintI64 => {
                self.check_builtin_args(arguments, &[Type::I64], env, context, span);
                Type::Unit
            }
            Builtin::IoPrintBytes | Builtin::IoPrintln => {
                self.check_borrowing_args(arguments, &[Type::Bytes], env, context, span);
                Type::Unit
            }
            Builtin::IoReadFile => {
                self.check_borrowing_args(arguments, &[Type::Bytes], env, context, span);
                Type::Bytes
            }
            Builtin::VecNew => {
                self.check_builtin_args(arguments, &[], env, context, span);
                match expected {
                    Some(Type::Vec(inner)) => Type::Vec(inner.clone()),
                    _ => {
                        self.diagnostics.push(
                            Diagnostic::error(
                                "E0318",
                                "vec.new requires an expected Vec type",
                                span,
                            )
                            .note("bind it with (let values (Vec I64) (call vec.new) ...)"),
                        );
                        Type::Error
                    }
                }
            }
            Builtin::VecLen => {
                if arguments.len() != 1 {
                    self.wrong_builtin_arity(builtin.name(), 1, arguments.len(), span);
                    return Type::I64;
                }
                let argument_type =
                    self.check_expr(&mut arguments[0], env, context, None, Use::Borrow, false);
                if !matches!(argument_type, Type::Vec(_)) && argument_type != Type::Error {
                    self.expected_diagnostic("Vec", &argument_type, arguments[0].span);
                }
                Type::I64
            }
            Builtin::VecGet => {
                if arguments.len() != 2 {
                    self.wrong_builtin_arity(builtin.name(), 2, arguments.len(), span);
                    return Type::Error;
                }
                let vector_type =
                    self.check_expr(&mut arguments[0], env, context, None, Use::Borrow, false);
                let index_type = self.check_expr(
                    &mut arguments[1],
                    env,
                    context,
                    Some(&Type::I64),
                    Use::Move,
                    false,
                );
                self.expect_type(&index_type, &Type::I64, arguments[1].span);
                match vector_type {
                    Type::Vec(inner) => {
                        if !self.is_copy_type(&inner, &mut BTreeSet::new()) {
                            self.diagnostics.push(
                                Diagnostic::error(
                                    "E0319",
                                    "vec.get requires a copyable element type",
                                    arguments[0].span,
                                )
                                .note("store non-copyable values in an Arena and keep typed IDs"),
                            );
                        }
                        *inner
                    }
                    Type::Error => Type::Error,
                    other => {
                        self.expected_diagnostic("Vec", &other, arguments[0].span);
                        Type::Error
                    }
                }
            }
            Builtin::VecPush => self.check_vec_write(arguments, false, span, env, context),
            Builtin::VecSet => self.check_vec_write(arguments, true, span, env, context),
            Builtin::ArenaNew => {
                self.check_builtin_args(arguments, &[], env, context, span);
                match expected {
                    Some(Type::Arena(inner)) => Type::Arena(inner.clone()),
                    _ => {
                        self.diagnostics.push(Diagnostic::error(
                            "E0320",
                            "arena.new requires an expected Arena type",
                            span,
                        ));
                        Type::Error
                    }
                }
            }
            Builtin::ArenaAdd => self.check_arena_add(arguments, span, env, context),
            Builtin::ArenaGet => self.check_arena_get(arguments, span, env, context),
        }
    }

    fn check_builtin_args(
        &mut self,
        arguments: &mut [Expr],
        expected: &[Type],
        env: &mut Environment,
        context: &FunctionContext<'_>,
        span: Span,
    ) {
        self.check_mixed_args(
            arguments,
            &expected
                .iter()
                .cloned()
                .map(|ty| (ty, Use::Move))
                .collect::<Vec<_>>(),
            env,
            context,
            span,
        );
    }

    fn check_borrowing_args(
        &mut self,
        arguments: &mut [Expr],
        expected: &[Type],
        env: &mut Environment,
        context: &FunctionContext<'_>,
        span: Span,
    ) {
        self.check_mixed_args(
            arguments,
            &expected
                .iter()
                .cloned()
                .map(|ty| (ty, Use::Borrow))
                .collect::<Vec<_>>(),
            env,
            context,
            span,
        );
    }

    fn check_mixed_args(
        &mut self,
        arguments: &mut [Expr],
        expected: &[(Type, Use)],
        env: &mut Environment,
        context: &FunctionContext<'_>,
        span: Span,
    ) {
        if arguments.len() != expected.len() {
            self.diagnostics.push(Diagnostic::error(
                "E0321",
                format!(
                    "built-in expects {} argument(s), found {}",
                    expected.len(),
                    arguments.len()
                ),
                span,
            ));
        }
        for (argument, (expected_type, usage)) in arguments.iter_mut().zip(expected) {
            let actual =
                self.check_expr(argument, env, context, Some(expected_type), *usage, false);
            self.expect_type(&actual, expected_type, argument.span);
        }
    }

    fn check_vec_write(
        &mut self,
        arguments: &mut [Expr],
        has_index: bool,
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
    ) -> Type {
        let expected_len = if has_index { 3 } else { 2 };
        if arguments.len() != expected_len {
            self.wrong_builtin_arity(
                if has_index { "vec.set" } else { "vec.push" },
                expected_len,
                arguments.len(),
                span,
            );
            return Type::Unit;
        }
        if !matches!(arguments[0].kind, ExprKind::Name(_)) {
            self.diagnostics.push(
                Diagnostic::error(
                    "E0346",
                    "growing or updating a vector requires a named unique binding",
                    arguments[0].span,
                )
                .note("bind the vector with let before mutating it"),
            );
        }
        let vector_type =
            self.check_expr(&mut arguments[0], env, context, None, Use::Borrow, false);
        let value_index = if has_index { 2 } else { 1 };
        if has_index {
            let index_type = self.check_expr(
                &mut arguments[1],
                env,
                context,
                Some(&Type::I64),
                Use::Move,
                false,
            );
            self.expect_type(&index_type, &Type::I64, arguments[1].span);
        }
        match vector_type {
            Type::Vec(inner) => {
                let value_type = self.check_expr(
                    &mut arguments[value_index],
                    env,
                    context,
                    Some(&inner),
                    Use::Move,
                    false,
                );
                self.expect_type(&value_type, &inner, arguments[value_index].span);
            }
            Type::Error => {}
            other => self.expected_diagnostic("Vec", &other, arguments[0].span),
        }
        Type::Unit
    }

    fn check_arena_add(
        &mut self,
        arguments: &mut [Expr],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
    ) -> Type {
        if arguments.len() != 2 {
            self.wrong_builtin_arity("arena.add", 2, arguments.len(), span);
            return Type::Error;
        }
        if !matches!(arguments[0].kind, ExprKind::Name(_)) {
            self.diagnostics.push(
                Diagnostic::error(
                    "E0346",
                    "growing an arena requires a named unique binding",
                    arguments[0].span,
                )
                .note("bind the arena with let before mutating it"),
            );
        }
        let arena_type = self.check_expr(&mut arguments[0], env, context, None, Use::Borrow, false);
        match arena_type {
            Type::Arena(inner) => {
                let value_type = self.check_expr(
                    &mut arguments[1],
                    env,
                    context,
                    Some(&inner),
                    Use::Move,
                    false,
                );
                self.expect_type(&value_type, &inner, arguments[1].span);
                Type::Id(inner)
            }
            Type::Error => Type::Error,
            other => {
                self.expected_diagnostic("Arena", &other, arguments[0].span);
                Type::Error
            }
        }
    }

    fn check_arena_get(
        &mut self,
        arguments: &mut [Expr],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
    ) -> Type {
        if arguments.len() != 2 {
            self.wrong_builtin_arity("arena.get", 2, arguments.len(), span);
            return Type::Error;
        }
        let arena_type = self.check_expr(&mut arguments[0], env, context, None, Use::Borrow, false);
        let id_type = self.check_expr(&mut arguments[1], env, context, None, Use::Move, false);
        match (arena_type, id_type) {
            (Type::Arena(inner), Type::Id(id_inner)) if inner == id_inner => {
                if !self.is_copy_type(&inner, &mut BTreeSet::new()) {
                    self.diagnostics.push(
                        Diagnostic::error(
                            "E0322",
                            "arena.get requires a copyable value type",
                            span,
                        )
                        .note("arena values used by ID should contain scalars and typed IDs"),
                    );
                }
                *inner
            }
            (Type::Error, _) | (_, Type::Error) => Type::Error,
            (arena, id) => {
                self.diagnostics.push(Diagnostic::error(
                    "E0323",
                    format!(
                        "arena.get type mismatch: arena is `{}`, id is `{}`",
                        arena.display(),
                        id.display()
                    ),
                    span,
                ));
                Type::Error
            }
        }
    }

    fn wrong_builtin_arity(&mut self, name: &str, expected: usize, actual: usize, span: Span) {
        self.diagnostics.push(Diagnostic::error(
            "E0324",
            format!("`{name}` expects {expected} argument(s), found {actual}"),
            span,
        ));
    }

    fn check_make(
        &mut self,
        record_name: &str,
        fields: &mut [(String, Expr, Span)],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
    ) -> Type {
        let Some(record) = self.records.get(record_name).cloned() else {
            self.diagnostics.push(Diagnostic::error(
                "E0325",
                format!("unknown record `{record_name}`"),
                span,
            ));
            return Type::Error;
        };
        let declared: BTreeMap<_, _> = record
            .fields
            .iter()
            .map(|field| (field.name.clone(), field))
            .collect();
        let mut initialized = BTreeSet::new();
        for (name, value, field_span) in fields {
            if !initialized.insert(name.clone()) {
                self.diagnostics.push(Diagnostic::error(
                    "E0326",
                    format!("field `{name}` initialized more than once"),
                    *field_span,
                ));
            }
            let Some(field) = declared.get(name) else {
                self.diagnostics.push(Diagnostic::error(
                    "E0327",
                    format!("record `{record_name}` has no field `{name}`"),
                    *field_span,
                ));
                self.check_expr(value, env, context, None, Use::Move, false);
                continue;
            };
            let value_type =
                self.check_expr(value, env, context, Some(&field.ty), Use::Move, false);
            self.expect_type(&value_type, &field.ty, value.span);
        }
        let missing: Vec<_> = declared
            .keys()
            .filter(|field| !initialized.contains(*field))
            .cloned()
            .collect();
        if !missing.is_empty() {
            self.diagnostics.push(Diagnostic::error(
                "E0328",
                format!("missing record field(s): {}", missing.join(", ")),
                span,
            ));
        }
        Type::Named(record_name.to_owned())
    }

    fn check_get(&mut self, value_type: &Type, field_name: &str, span: Span) -> Type {
        let Type::Named(record_name) = value_type else {
            if value_type != &Type::Error {
                self.diagnostics.push(Diagnostic::error(
                    "E0329",
                    format!(
                        "field projection requires a record, found `{}`",
                        value_type.display()
                    ),
                    span,
                ));
            }
            return Type::Error;
        };
        let Some(record) = self.records.get(record_name) else {
            self.diagnostics.push(Diagnostic::error(
                "E0329",
                format!("`{record_name}` is not a record"),
                span,
            ));
            return Type::Error;
        };
        let Some(field) = record.fields.iter().find(|field| field.name == field_name) else {
            self.diagnostics.push(Diagnostic::error(
                "E0330",
                format!("record `{record_name}` has no field `{field_name}`"),
                span,
            ));
            return Type::Error;
        };
        field.ty.clone()
    }

    fn check_case(
        &mut self,
        variant_name: &str,
        case_name: &str,
        payload: &mut [Expr],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
    ) -> Type {
        let Some(variant) = self.variants.get(variant_name).cloned() else {
            self.diagnostics.push(Diagnostic::error(
                "E0331",
                format!("unknown variant `{variant_name}`"),
                span,
            ));
            return Type::Error;
        };
        let Some(case) = variant.cases.iter().find(|case| case.name == case_name) else {
            self.diagnostics.push(Diagnostic::error(
                "E0332",
                format!("variant `{variant_name}` has no case `{case_name}`"),
                span,
            ));
            return Type::Error;
        };
        if payload.len() != case.payload.len() {
            self.diagnostics.push(Diagnostic::error(
                "E0333",
                format!(
                    "case `{case_name}` expects {} value(s), found {}",
                    case.payload.len(),
                    payload.len()
                ),
                span,
            ));
        }
        for (value, expected) in payload.iter_mut().zip(&case.payload) {
            let actual = self.check_expr(value, env, context, Some(expected), Use::Move, false);
            self.expect_type(&actual, expected, value.span);
        }
        Type::Named(variant_name.to_owned())
    }

    #[allow(clippy::too_many_arguments)]
    fn check_match(
        &mut self,
        value: &mut Expr,
        arms: &mut [MatchArm],
        span: Span,
        env: &mut Environment,
        context: &FunctionContext<'_>,
        expected: Option<&Type>,
        usage: Use,
        tail: bool,
    ) -> Type {
        let value_type = self.check_expr(value, env, context, None, Use::Move, false);
        let before_arms = env.clone();
        let mut result_type = expected.cloned();
        let mut moved_in_any_arm = BTreeSet::new();
        match &value_type {
            Type::Bool => {
                let mut seen = [false, false];
                for arm in arms {
                    let Pattern::Bool(value) = arm.pattern else {
                        self.diagnostics.push(Diagnostic::error(
                            "E0334",
                            "Boolean match requires true and false patterns",
                            arm.span,
                        ));
                        continue;
                    };
                    let index = usize::from(value);
                    if seen[index] {
                        self.diagnostics.push(Diagnostic::error(
                            "E0335",
                            "duplicate Boolean match arm",
                            arm.span,
                        ));
                    }
                    seen[index] = true;
                    let mut arm_env = before_arms.clone();
                    let arm_type = self.check_expr(
                        &mut arm.body,
                        &mut arm_env,
                        context,
                        result_type.as_ref(),
                        usage,
                        tail,
                    );
                    self.unify_match_type(&mut result_type, &arm_type, arm.body.span);
                    moved_in_any_arm.extend(arm_env.moved_names());
                }
                if seen != [true, true] {
                    self.diagnostics.push(
                        Diagnostic::error("E0336", "non-exhaustive Boolean match", span)
                            .note("provide both (true ...) and (false ...) arms"),
                    );
                }
            }
            Type::Named(variant_name) if self.variants.contains_key(variant_name) => {
                let variant = self.variants.get(variant_name).cloned().unwrap();
                let mut seen = BTreeSet::new();
                for arm in arms {
                    let Pattern::Variant { case, bindings } = &arm.pattern else {
                        self.diagnostics.push(Diagnostic::error(
                            "E0337",
                            "variant match requires variant-case patterns",
                            arm.span,
                        ));
                        continue;
                    };
                    if !seen.insert(case.clone()) {
                        self.diagnostics.push(Diagnostic::error(
                            "E0338",
                            format!("duplicate match arm `{case}`"),
                            arm.span,
                        ));
                    }
                    let Some(case_info) = variant.cases.iter().find(|item| item.name == *case)
                    else {
                        self.diagnostics.push(Diagnostic::error(
                            "E0339",
                            format!("variant `{variant_name}` has no case `{case}`"),
                            arm.span,
                        ));
                        continue;
                    };
                    if bindings.len() != case_info.payload.len() {
                        self.diagnostics.push(Diagnostic::error(
                            "E0340",
                            format!(
                                "case `{case}` binds {} value(s), expected {}",
                                bindings.len(),
                                case_info.payload.len()
                            ),
                            arm.span,
                        ));
                    }
                    let mut arm_env = before_arms.clone();
                    for (binding, ty) in bindings.iter().zip(&case_info.payload) {
                        arm_env.bindings.push(Binding {
                            name: binding.clone(),
                            ty: ty.clone(),
                            declared_at: arm.span,
                            moved_at: None,
                        });
                    }
                    let arm_type = self.check_expr(
                        &mut arm.body,
                        &mut arm_env,
                        context,
                        result_type.as_ref(),
                        usage,
                        tail,
                    );
                    self.unify_match_type(&mut result_type, &arm_type, arm.body.span);
                    moved_in_any_arm.extend(arm_env.moved_names());
                }
                let missing: Vec<_> = variant
                    .cases
                    .iter()
                    .filter(|case| !seen.contains(&case.name))
                    .map(|case| case.name.clone())
                    .collect();
                if !missing.is_empty() {
                    self.diagnostics.push(Diagnostic::error(
                        "E0341",
                        format!("non-exhaustive match; missing {}", missing.join(", ")),
                        span,
                    ));
                }
            }
            Type::Error => {}
            other => self.diagnostics.push(Diagnostic::error(
                "E0342",
                format!("cannot match value of type `{}`", other.display()),
                value.span,
            )),
        }
        for binding in &mut env.bindings {
            if moved_in_any_arm.contains(&binding.name) && binding.moved_at.is_none() {
                binding.moved_at = Some(span);
            }
        }
        result_type.unwrap_or(Type::Error)
    }

    fn unify_match_type(&mut self, result: &mut Option<Type>, actual: &Type, span: Span) {
        match result {
            Some(expected) => self.expect_type(actual, expected, span),
            None => *result = Some(actual.clone()),
        }
    }

    fn require_effect(&mut self, effect: Effect, span: Span, context: &FunctionContext<'_>) {
        if !context.effects.contains(&effect) {
            self.diagnostics.push(
                Diagnostic::error(
                    "E0343",
                    format!(
                        "function `{}` does not declare `{}` effect",
                        context.name,
                        effect.name()
                    ),
                    span,
                )
                .fix(
                    span,
                    effect.name(),
                    format!("add `{}` to the function effect list", effect.name()),
                ),
            );
        }
    }

    fn expect_type(&mut self, actual: &Type, expected: &Type, span: Span) {
        if actual != expected && actual != &Type::Error && expected != &Type::Error {
            self.diagnostics.push(Diagnostic::error(
                "E0344",
                format!(
                    "type mismatch: expected `{}`, found `{}`",
                    expected.display(),
                    actual.display()
                ),
                span,
            ));
        }
    }

    fn expected_diagnostic(&mut self, expected: &str, actual: &Type, span: Span) {
        self.diagnostics.push(Diagnostic::error(
            "E0345",
            format!("expected `{expected}`, found `{}`", actual.display()),
            span,
        ));
    }

    fn is_copy_type(&self, ty: &Type, visiting: &mut BTreeSet<String>) -> bool {
        match ty {
            ty if ty.is_copy() => true,
            Type::Named(name) => {
                if !visiting.insert(name.clone()) {
                    return false;
                }
                let result = if let Some(record) = self.records.get(name) {
                    record
                        .fields
                        .iter()
                        .all(|field| self.is_copy_type(&field.ty, visiting))
                } else if let Some(variant) = self.variants.get(name) {
                    variant.cases.iter().all(|case| {
                        case.payload
                            .iter()
                            .all(|ty| self.is_copy_type(ty, visiting))
                    })
                } else {
                    false
                };
                visiting.remove(name);
                result
            }
            _ => false,
        }
    }
}

#[derive(Clone, Debug, Default)]
struct Environment {
    bindings: Vec<Binding>,
}

impl Environment {
    fn moved_names(&self) -> BTreeSet<String> {
        self.bindings
            .iter()
            .filter(|binding| binding.moved_at.is_some())
            .map(|binding| binding.name.clone())
            .collect()
    }
}

#[derive(Clone, Debug)]
struct Binding {
    name: String,
    ty: Type,
    declared_at: Span,
    moved_at: Option<Span>,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum Use {
    Borrow,
    Move,
}

struct FunctionContext<'a> {
    name: &'a str,
    params: &'a [Param],
    return_type: &'a Type,
    effects: &'a BTreeSet<Effect>,
}

#[derive(Clone, Copy, Debug)]
pub enum Builtin {
    I64Add,
    I64Sub,
    I64Mul,
    I64Div,
    I64Rem,
    I64Eq,
    I64Lt,
    I64Le,
    I64Gt,
    I64Ge,
    BoolNot,
    BoolAnd,
    BoolOr,
    U8ToI64,
    I64ToU8,
    BytesLen,
    BytesGet,
    IoPrintI64,
    IoPrintBytes,
    IoPrintln,
    IoReadFile,
    VecNew,
    VecLen,
    VecGet,
    VecPush,
    VecSet,
    ArenaNew,
    ArenaAdd,
    ArenaGet,
}

impl Builtin {
    pub fn from_name(name: &str) -> Option<Self> {
        Some(match name {
            "i64.add" => Self::I64Add,
            "i64.sub" => Self::I64Sub,
            "i64.mul" => Self::I64Mul,
            "i64.div" => Self::I64Div,
            "i64.rem" => Self::I64Rem,
            "i64.eq" => Self::I64Eq,
            "i64.lt" => Self::I64Lt,
            "i64.le" => Self::I64Le,
            "i64.gt" => Self::I64Gt,
            "i64.ge" => Self::I64Ge,
            "bool.not" => Self::BoolNot,
            "bool.and" => Self::BoolAnd,
            "bool.or" => Self::BoolOr,
            "u8.to-i64" => Self::U8ToI64,
            "i64.to-u8" => Self::I64ToU8,
            "bytes.len" => Self::BytesLen,
            "bytes.get" => Self::BytesGet,
            "io.print-i64" => Self::IoPrintI64,
            "io.print-bytes" => Self::IoPrintBytes,
            "io.println" => Self::IoPrintln,
            "io.read-file" => Self::IoReadFile,
            "vec.new" => Self::VecNew,
            "vec.len" => Self::VecLen,
            "vec.get" => Self::VecGet,
            "vec.push" => Self::VecPush,
            "vec.set" => Self::VecSet,
            "arena.new" => Self::ArenaNew,
            "arena.add" => Self::ArenaAdd,
            "arena.get" => Self::ArenaGet,
            _ => return None,
        })
    }

    pub fn name(self) -> &'static str {
        match self {
            Self::I64Add => "i64.add",
            Self::I64Sub => "i64.sub",
            Self::I64Mul => "i64.mul",
            Self::I64Div => "i64.div",
            Self::I64Rem => "i64.rem",
            Self::I64Eq => "i64.eq",
            Self::I64Lt => "i64.lt",
            Self::I64Le => "i64.le",
            Self::I64Gt => "i64.gt",
            Self::I64Ge => "i64.ge",
            Self::BoolNot => "bool.not",
            Self::BoolAnd => "bool.and",
            Self::BoolOr => "bool.or",
            Self::U8ToI64 => "u8.to-i64",
            Self::I64ToU8 => "i64.to-u8",
            Self::BytesLen => "bytes.len",
            Self::BytesGet => "bytes.get",
            Self::IoPrintI64 => "io.print-i64",
            Self::IoPrintBytes => "io.print-bytes",
            Self::IoPrintln => "io.println",
            Self::IoReadFile => "io.read-file",
            Self::VecNew => "vec.new",
            Self::VecLen => "vec.len",
            Self::VecGet => "vec.get",
            Self::VecPush => "vec.push",
            Self::VecSet => "vec.set",
            Self::ArenaNew => "arena.new",
            Self::ArenaAdd => "arena.add",
            Self::ArenaGet => "arena.get",
        }
    }

    fn effects(self) -> &'static [Effect] {
        match self {
            Self::IoPrintI64 | Self::IoPrintBytes | Self::IoPrintln => &[Effect::Io],
            Self::IoReadFile => &[Effect::Alloc, Effect::Io],
            Self::VecNew | Self::VecPush | Self::ArenaNew | Self::ArenaAdd => &[Effect::Alloc],
            _ => &[],
        }
    }
}

fn is_reserved_function(name: &str) -> bool {
    Builtin::from_name(name).is_some()
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{lexer, parser, sexpr};

    fn checked(source: &str) -> (Option<CheckedProgram>, Vec<Diagnostic>) {
        let (tokens, mut diagnostics) = lexer::lex(source);
        let (forms, parse_diagnostics) = sexpr::parse(&tokens, source.len());
        diagnostics.extend(parse_diagnostics);
        let (program, lower_diagnostics) = parser::lower(&forms);
        diagnostics.extend(lower_diagnostics);
        let Some(program) = program else {
            return (None, diagnostics);
        };
        let (program, check_diagnostics) = check(program);
        diagnostics.extend(check_diagnostics);
        (program, diagnostics)
    }

    #[test]
    fn checks_calls_and_effects() {
        let (program, diagnostics) = checked(
            "(module hello (fn main () I64 (effects io) (let shown Unit (call io.print-i64 42) 0)))",
        );
        assert!(program.is_some(), "{diagnostics:#?}");
        assert!(diagnostics.is_empty(), "{diagnostics:#?}");
    }

    #[test]
    fn rejects_missing_effect() {
        let (_, diagnostics) = checked(
            "(module hello (fn main () I64 (effects) (let shown Unit (call io.print-i64 42) 0)))",
        );
        assert!(
            diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0343")
        );
    }

    #[test]
    fn checks_exhaustive_boolean_match() {
        let (program, diagnostics) =
            checked("(module hello (fn main () I64 (effects) (match true (true 1) (false 0))))");
        assert!(program.is_some(), "{diagnostics:#?}");
    }

    #[test]
    fn rejects_non_exhaustive_match() {
        let (_, diagnostics) =
            checked("(module hello (fn main () I64 (effects) (match true (true 1))))");
        assert!(
            diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0336")
        );
    }

    #[test]
    fn rejects_use_after_move() {
        let (_, diagnostics) = checked(
            "(module hello (fn consume ((x Bytes)) Unit (effects) unit) (fn main () I64 (effects) (let data Bytes \"x\" (let used Unit (call consume data) (let again Unit (call consume data) 0)))))",
        );
        assert!(
            diagnostics
                .iter()
                .any(|diagnostic| diagnostic.code == "E0315")
        );
    }
}
