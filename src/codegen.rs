use std::collections::BTreeMap;
use std::fmt::Write as _;

use crate::ast::*;
use crate::sema::{Builtin, CheckedProgram};

pub fn generate_c(program: &CheckedProgram) -> String {
    generate_c_for_entry(program, "main")
}

pub fn generate_c_for_entry(program: &CheckedProgram, entry: &str) -> String {
    let fragments = program
        .program
        .items
        .iter()
        .map(|item| (item_name(item).to_owned(), generate_item_c(program, item)))
        .collect();
    generate_c_from_fragments_for_entry(program, &fragments, entry)
}

pub(crate) fn generate_project_module_fragment(program: &CheckedProgram, module: &str) -> String {
    let prefix = format!("{module}/");
    let fragments = program
        .program
        .items
        .iter()
        .filter(|item| item_name(item).starts_with(&prefix))
        .map(|item| (item_name(item).to_owned(), generate_item_c(program, item)))
        .collect();
    assemble_project_module_fragment(program, module, &fragments)
}

pub(crate) fn assemble_project_module_fragment(
    program: &CheckedProgram,
    module: &str,
    fragments: &BTreeMap<String, String>,
) -> String {
    let prefix = format!("{module}/");
    let selected: Vec<_> = program
        .program
        .items
        .iter()
        .filter(|item| item_name(item).starts_with(&prefix))
        .collect();
    let mut output = String::new();
    for item in &selected {
        match item {
            Item::Record(record) => {
                writeln!(
                    output,
                    "typedef struct {} {};",
                    c_type_name(&record.name),
                    c_type_name(&record.name)
                )
                .unwrap();
            }
            Item::Variant(variant) => {
                writeln!(
                    output,
                    "typedef struct {} {};",
                    c_type_name(&variant.name),
                    c_type_name(&variant.name)
                )
                .unwrap();
            }
            Item::Function(_) => {}
        }
    }
    if selected
        .iter()
        .any(|item| matches!(item, Item::Record(_) | Item::Variant(_)))
    {
        output.push('\n');
    }
    for item in &selected {
        if matches!(item, Item::Record(_) | Item::Variant(_)) {
            output.push_str(fragment(fragments, item_name(item)));
        }
    }
    for item in &selected {
        if let Item::Function(function) = item {
            emit_function_prototype(&mut output, function);
        }
    }
    if selected
        .iter()
        .any(|item| matches!(item, Item::Function(_)))
    {
        output.push('\n');
    }
    for item in &selected {
        if matches!(item, Item::Function(_)) {
            output.push_str(fragment(fragments, item_name(item)));
        }
    }
    output
}

pub(crate) fn assemble_project_c(
    fragments: &BTreeMap<String, String>,
    modules: &[String],
    entry: &str,
) -> String {
    let mut output = generated_header();
    for module in modules {
        output.push_str(
            fragments
                .get(module)
                .unwrap_or_else(|| panic!("missing generated module fragment for {module}")),
        );
    }
    emit_main_wrapper(&mut output, entry);
    output
}

pub(crate) fn generate_c_from_fragments(
    program: &CheckedProgram,
    fragments: &BTreeMap<String, String>,
) -> String {
    generate_c_from_fragments_for_entry(program, fragments, "main")
}

pub(crate) fn generate_c_from_fragments_for_entry(
    program: &CheckedProgram,
    fragments: &BTreeMap<String, String>,
    entry: &str,
) -> String {
    let mut output = generated_header();

    for record in program.records.values() {
        writeln!(
            output,
            "typedef struct {} {};",
            c_type_name(&record.name),
            c_type_name(&record.name)
        )
        .unwrap();
    }
    for variant in program.variants.values() {
        writeln!(
            output,
            "typedef struct {} {};",
            c_type_name(&variant.name),
            c_type_name(&variant.name)
        )
        .unwrap();
    }
    if !program.records.is_empty() || !program.variants.is_empty() {
        output.push('\n');
    }

    for item in &program.program.items {
        match item {
            Item::Record(record) => output.push_str(fragment(fragments, &record.name)),
            Item::Variant(variant) => output.push_str(fragment(fragments, &variant.name)),
            Item::Function(_) => {}
        }
    }

    for item in &program.program.items {
        if let Item::Function(function) = item {
            emit_function_prototype(&mut output, function);
        }
    }
    output.push('\n');

    for item in &program.program.items {
        if let Item::Function(function) = item {
            output.push_str(fragment(fragments, &function.name));
        }
    }

    emit_main_wrapper(&mut output, entry);
    output
}

fn generated_header() -> String {
    let mut output = String::new();
    writeln!(
        output,
        "/* generated deterministically by slimc {} */",
        crate::VERSION
    )
    .unwrap();
    output.push_str("#include \"slim_rt.h\"\n#include <string.h>\n\n");
    output
}

fn emit_main_wrapper(output: &mut String, entry: &str) {
    output.push_str("int main(int argc, char **argv) {\n");
    output.push_str("    slim_rt_init();\n");
    output.push_str("    SlimVec slim_args = slim_vec_new(sizeof(SlimBytes));\n");
    output.push_str("    for (int slim_i = 0; slim_i < argc; ++slim_i) {\n");
    output.push_str("        SlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));\n");
    output.push_str("        slim_vec_push(&slim_args, &slim_arg);\n");
    output.push_str("    }\n");
    writeln!(
        output,
        "    int64_t slim_exit_code = {}(slim_args);",
        c_function_name(entry)
    )
    .unwrap();
    output.push_str("    if (slim_exit_code < 0 || slim_exit_code > 255) {\n");
    output.push_str("        slim_rt_trap(\"main result is outside 0..255\");\n");
    output.push_str("    }\n");
    output.push_str("    slim_rt_shutdown();\n");
    output.push_str("    return (int)slim_exit_code;\n");
    output.push_str("}\n");
}

pub(crate) fn generate_item_c(program: &CheckedProgram, item: &Item) -> String {
    let mut output = String::new();
    match item {
        Item::Record(record) => emit_record(&mut output, record),
        Item::Variant(variant) => emit_variant(&mut output, variant),
        Item::Function(function) => {
            let mut emitter = FunctionEmitter::new(program, function);
            emitter.emit_function(&mut output);
        }
    }
    output
}

fn item_name(item: &Item) -> &str {
    match item {
        Item::Function(function) => &function.name,
        Item::Record(record) => &record.name,
        Item::Variant(variant) => &variant.name,
    }
}

fn fragment<'a>(fragments: &'a BTreeMap<String, String>, name: &str) -> &'a str {
    fragments
        .get(name)
        .unwrap_or_else(|| panic!("missing generated declaration fragment for {name}"))
}

fn emit_record(output: &mut String, record: &Record) {
    writeln!(output, "struct {} {{", c_type_name(&record.name)).unwrap();
    if record.fields.is_empty() {
        output.push_str("    SlimUnit slim_unit;\n");
    } else {
        for field in &record.fields {
            writeln!(
                output,
                "    {} {};",
                c_type(&field.ty),
                c_field_name(&field.name)
            )
            .unwrap();
        }
    }
    output.push_str("};\n\n");
}

fn emit_variant(output: &mut String, variant: &Variant) {
    for (index, case) in variant.cases.iter().enumerate() {
        writeln!(
            output,
            "#define {} ((int32_t){index})",
            c_tag_name(&variant.name, &case.name)
        )
        .unwrap();
    }
    writeln!(output, "struct {} {{", c_type_name(&variant.name)).unwrap();
    output.push_str("    int32_t tag;\n");
    output.push_str("    union {\n");
    output.push_str("        uint8_t slim_unused;\n");
    for case in &variant.cases {
        if case.payload.is_empty() {
            continue;
        }
        output.push_str("        struct {\n");
        for (index, ty) in case.payload.iter().enumerate() {
            writeln!(output, "            {} slim_{index};", c_type(ty)).unwrap();
        }
        writeln!(output, "        }} {};", c_case_name(&case.name)).unwrap();
    }
    output.push_str("    } data;\n");
    output.push_str("};\n\n");
}

fn emit_function_prototype(output: &mut String, function: &Function) {
    write!(
        output,
        "static {} {}(",
        c_type(&function.return_type),
        c_function_name(&function.name)
    )
    .unwrap();
    if function.params.is_empty() {
        output.push_str("void");
    } else {
        for (index, param) in function.params.iter().enumerate() {
            if index > 0 {
                output.push_str(", ");
            }
            write!(
                output,
                "{}{} slim_arg_{index}",
                c_type(&param.ty),
                if param.mode == ParamMode::Inout {
                    " *"
                } else {
                    ""
                }
            )
            .unwrap();
        }
    }
    output.push_str(");\n");
}

struct FunctionEmitter<'a> {
    program: &'a CheckedProgram,
    function: &'a Function,
    code: String,
    indent: usize,
    next_temp: usize,
    bindings: Vec<(String, String)>,
    parameter_c_names: Vec<String>,
}

impl<'a> FunctionEmitter<'a> {
    fn new(program: &'a CheckedProgram, function: &'a Function) -> Self {
        Self {
            program,
            function,
            code: String::new(),
            indent: 0,
            next_temp: 0,
            bindings: Vec::new(),
            parameter_c_names: Vec::new(),
        }
    }

    fn emit_function(&mut self, output: &mut String) {
        write!(
            self.code,
            "static {} {}(",
            c_type(&self.function.return_type),
            c_function_name(&self.function.name)
        )
        .unwrap();
        if self.function.params.is_empty() {
            self.code.push_str("void");
        } else {
            for (index, param) in self.function.params.iter().enumerate() {
                if index > 0 {
                    self.code.push_str(", ");
                }
                write!(
                    self.code,
                    "{}{} slim_arg_{index}",
                    c_type(&param.ty),
                    if param.mode == ParamMode::Inout {
                        " *"
                    } else {
                        ""
                    }
                )
                .unwrap();
            }
        }
        self.code.push_str(") {\n");
        self.indent = 1;

        for (index, param) in self.function.params.iter().enumerate() {
            let local = if param.mode == ParamMode::Inout {
                self.line(&format!("(void)slim_arg_{index};"));
                format!("(*slim_arg_{index})")
            } else {
                let local = self.fresh_named(&param.name);
                self.line(&format!(
                    "{} {} = slim_arg_{index};",
                    c_type(&param.ty),
                    local
                ));
                self.line(&format!("(void){local};"));
                local
            };
            self.bindings.push((param.name.clone(), local.clone()));
            self.parameter_c_names.push(local);
        }
        self.line(&format!(
            "{} slim_result = {{0}};",
            c_type(&self.function.return_type)
        ));
        if contains_recur(&self.function.body) {
            self.code.push_str("slim_recur: ;\n");
        }
        self.emit_expr(&self.function.body, "slim_result");
        self.line("return slim_result;");
        self.code.push_str("}\n\n");
        output.push_str(&self.code);
    }

    fn emit_expr(&mut self, expr: &Expr, destination: &str) {
        match &expr.kind {
            ExprKind::Unit => self.line(&format!("{destination} = (SlimUnit){{0}};")),
            ExprKind::Bool(value) => self.line(&format!(
                "{destination} = {};",
                if *value { "true" } else { "false" }
            )),
            ExprKind::I64(value) => self.line(&format!("{destination} = INT64_C({value});")),
            ExprKind::Bytes(value) => self.line(&format!(
                "{destination} = slim_bytes_static((const uint8_t *){}, INT64_C({}));",
                c_byte_string(value),
                value.len()
            )),
            ExprKind::Name(name) => {
                let binding = self.binding(name);
                self.line(&format!("{destination} = {binding};"));
            }
            ExprKind::Let {
                name,
                declared_type,
                value,
                body,
            } => {
                let local = self.fresh_named(name);
                self.line("{");
                self.indent += 1;
                self.line(&format!("{} {} = {{0}};", c_type(declared_type), local));
                self.emit_expr(value, &local);
                self.line(&format!("(void){local};"));
                self.bindings.push((name.clone(), local));
                self.emit_expr(body, destination);
                self.bindings.pop();
                self.indent -= 1;
                self.line("}");
            }
            ExprKind::Call {
                function,
                arguments,
            } => self.emit_call(function, arguments, &expr.ty, destination),
            ExprKind::Recur(arguments) => {
                let mut temporaries = Vec::new();
                for argument in arguments {
                    temporaries.push(self.evaluate(argument));
                }
                for (parameter, temporary) in
                    self.parameter_c_names.clone().into_iter().zip(temporaries)
                {
                    self.line(&format!("{parameter} = {temporary};"));
                }
                self.line("goto slim_recur;");
            }
            ExprKind::Set { name, value, body } => {
                let temporary = self.evaluate(value);
                let binding = self.binding(name);
                self.line(&format!("{binding} = {temporary};"));
                self.emit_expr(body, destination);
            }
            ExprKind::Make { record, fields } => {
                let mut values = Vec::new();
                for (field, value, _) in fields {
                    values.push((field.clone(), self.evaluate(value)));
                }
                let initializers = values
                    .iter()
                    .map(|(field, value)| format!(".{} = {value}", c_field_name(field)))
                    .collect::<Vec<_>>()
                    .join(", ");
                self.line(&format!(
                    "{destination} = ({}){{{initializers}}};",
                    c_type_name(record)
                ));
            }
            ExprKind::Get { value, field } => {
                let temporary = self.evaluate(value);
                self.line(&format!(
                    "{destination} = {temporary}.{};",
                    c_field_name(field)
                ));
            }
            ExprKind::Case {
                variant,
                case,
                payload,
            } => {
                let temporaries: Vec<_> =
                    payload.iter().map(|value| self.evaluate(value)).collect();
                if temporaries.is_empty() {
                    self.line(&format!(
                        "{destination} = ({}){{.tag = {}}};",
                        c_type_name(variant),
                        c_tag_name(variant, case)
                    ));
                } else {
                    let fields = temporaries
                        .iter()
                        .enumerate()
                        .map(|(index, value)| format!(".slim_{index} = {value}"))
                        .collect::<Vec<_>>()
                        .join(", ");
                    self.line(&format!(
                        "{destination} = ({}){{.tag = {}, .data.{} = {{{fields}}}}};",
                        c_type_name(variant),
                        c_tag_name(variant, case),
                        c_case_name(case)
                    ));
                }
            }
            ExprKind::Match { value, arms } => self.emit_match(value, arms, destination),
            ExprKind::Error => self.line("slim_rt_trap(\"compiler emitted error expression\");"),
        }
    }

    fn emit_call(&mut self, name: &str, arguments: &[Expr], ty: &Type, destination: &str) {
        let builtin = Builtin::from_name(name);
        if builtin.is_none() {
            let modes = self
                .program
                .functions
                .get(name)
                .expect("checked function")
                .param_modes
                .clone();
            let mut c_arguments = Vec::new();
            for (argument, mode) in arguments.iter().zip(modes) {
                if mode == ParamMode::Inout {
                    let ExprKind::Name(source_name) = &argument.kind else {
                        unreachable!("checked inout argument")
                    };
                    c_arguments.push(format!("&({})", self.binding(source_name)));
                } else {
                    c_arguments.push(self.evaluate(argument));
                }
            }
            self.line(&format!(
                "{destination} = {}({});",
                c_function_name(name),
                c_arguments.join(", ")
            ));
            return;
        }
        if matches!(builtin, Some(Builtin::VecPush | Builtin::ArenaAdd)) {
            self.emit_mutating_growth_call(builtin.unwrap(), arguments, destination);
            return;
        }
        let temporaries: Vec<_> = arguments
            .iter()
            .map(|argument| self.evaluate(argument))
            .collect();
        match builtin {
            None => unreachable!(),
            Some(Builtin::I64Add) => self.assign_call(destination, "slim_i64_add", &temporaries),
            Some(Builtin::I64Sub) => self.assign_call(destination, "slim_i64_sub", &temporaries),
            Some(Builtin::I64Mul) => self.assign_call(destination, "slim_i64_mul", &temporaries),
            Some(Builtin::I64Div) => self.assign_call(destination, "slim_i64_div", &temporaries),
            Some(Builtin::I64Rem) => self.assign_call(destination, "slim_i64_rem", &temporaries),
            Some(Builtin::I64Eq) => self.assign_binary(destination, &temporaries, "=="),
            Some(Builtin::I64Lt) => self.assign_binary(destination, &temporaries, "<"),
            Some(Builtin::I64Le) => self.assign_binary(destination, &temporaries, "<="),
            Some(Builtin::I64Gt) => self.assign_binary(destination, &temporaries, ">"),
            Some(Builtin::I64Ge) => self.assign_binary(destination, &temporaries, ">="),
            Some(Builtin::BoolNot) => self.line(&format!("{destination} = !{};", temporaries[0])),
            Some(Builtin::BoolAnd) => self.assign_binary(destination, &temporaries, "&&"),
            Some(Builtin::BoolOr) => self.assign_binary(destination, &temporaries, "||"),
            Some(Builtin::U8ToI64) => {
                self.line(&format!("{destination} = (int64_t){};", temporaries[0]))
            }
            Some(Builtin::I64ToU8) => self.assign_call(destination, "slim_i64_to_u8", &temporaries),
            Some(Builtin::BytesLen) => {
                self.assign_call(destination, "slim_bytes_len", &temporaries)
            }
            Some(Builtin::BytesGet) => {
                self.assign_call(destination, "slim_bytes_get", &temporaries)
            }
            Some(Builtin::BytesFreeze) => {
                self.assign_call(destination, "slim_bytes_freeze", &temporaries)
            }
            Some(Builtin::IoPrintI64) => {
                self.assign_call(destination, "slim_print_i64", &temporaries)
            }
            Some(Builtin::IoPrintBytes) => {
                self.assign_call(destination, "slim_print_bytes", &temporaries)
            }
            Some(Builtin::IoPrintln) => self.assign_call(destination, "slim_println", &temporaries),
            Some(Builtin::IoReadFile) => {
                self.assign_call(destination, "slim_read_file", &temporaries)
            }
            Some(Builtin::VecNew) => {
                let Type::Vec(inner) = ty else {
                    unreachable!("checked vec.new has Vec result")
                };
                self.line(&format!(
                    "{destination} = slim_vec_new(sizeof({}));",
                    c_type(inner)
                ));
            }
            Some(Builtin::VecLen) => self.line(&format!("{destination} = {}.len;", temporaries[0])),
            Some(Builtin::VecGet) => {
                let checked_index = self.fresh_temp();
                self.line(&format!(
                    "size_t {checked_index} = slim_vec_check_index(&{}, {});",
                    temporaries[0], temporaries[1]
                ));
                self.line(&format!(
                    "{destination} = (({} *){}.data)[{checked_index}];",
                    c_type(ty),
                    temporaries[0]
                ));
            }
            Some(Builtin::VecSet) => {
                let checked_index = self.fresh_temp();
                self.line(&format!(
                    "size_t {checked_index} = slim_vec_check_index(&{}, {});",
                    temporaries[0], temporaries[1]
                ));
                self.line(&format!(
                    "(({} *){}.data)[{checked_index}] = {}; {destination} = (SlimUnit){{0}};",
                    c_type(&arguments[2].ty),
                    temporaries[0],
                    temporaries[2]
                ));
            }
            Some(Builtin::ArenaNew) => {
                let Type::Arena(inner) = ty else {
                    unreachable!("checked arena.new has Arena result")
                };
                self.line(&format!(
                    "{destination} = slim_vec_new(sizeof({}));",
                    c_type(inner)
                ));
            }
            Some(Builtin::ArenaGet) => {
                let checked_index = self.fresh_temp();
                self.line(&format!(
                    "size_t {checked_index} = slim_vec_check_index(&{}, {});",
                    temporaries[0], temporaries[1]
                ));
                self.line(&format!(
                    "{destination} = (({} *){}.data)[{checked_index}];",
                    c_type(ty),
                    temporaries[0]
                ));
            }
            Some(Builtin::VecPush | Builtin::ArenaAdd) => unreachable!(),
        }
    }

    fn emit_mutating_growth_call(
        &mut self,
        builtin: Builtin,
        arguments: &[Expr],
        destination: &str,
    ) {
        let ExprKind::Name(name) = &arguments[0].kind else {
            unreachable!("mutating growth built-in requires a binding")
        };
        let vector = self.binding(name);
        let value = self.evaluate(&arguments[1]);
        match builtin {
            Builtin::VecPush => self.line(&format!(
                "slim_vec_push(&{vector}, &{value}); {destination} = (SlimUnit){{0}};"
            )),
            Builtin::ArenaAdd => self.line(&format!(
                "{destination} = slim_arena_add(&{vector}, &{value});"
            )),
            _ => unreachable!(),
        }
    }

    fn emit_match(&mut self, value: &Expr, arms: &[MatchArm], destination: &str) {
        let scrutinee = self.evaluate(value);
        match &value.ty {
            Type::Bool => {
                for (index, arm) in arms.iter().enumerate() {
                    let Pattern::Bool(pattern) = arm.pattern else {
                        continue;
                    };
                    if index == 0 {
                        self.line(&format!(
                            "if ({}{}) {{",
                            if pattern { "" } else { "!" },
                            scrutinee
                        ));
                    } else {
                        self.line("else {");
                    }
                    self.indent += 1;
                    self.emit_expr(&arm.body, destination);
                    self.indent -= 1;
                    self.line("}");
                }
            }
            Type::Named(variant_name) => {
                self.line(&format!("switch ({scrutinee}.tag) {{"));
                self.indent += 1;
                for arm in arms {
                    let Pattern::Variant { case, bindings } = &arm.pattern else {
                        continue;
                    };
                    self.line(&format!("case {}: {{", c_tag_name(variant_name, case)));
                    self.indent += 1;
                    let case_info = self
                        .program
                        .variants
                        .get(variant_name)
                        .and_then(|variant| variant.cases.iter().find(|item| item.name == *case))
                        .expect("checked variant case");
                    for (index, (binding, ty)) in
                        bindings.iter().zip(&case_info.payload).enumerate()
                    {
                        let local = self.fresh_named(binding);
                        self.line(&format!(
                            "{} {} = {scrutinee}.data.{}.slim_{index};",
                            c_type(ty),
                            local,
                            c_case_name(case)
                        ));
                        self.bindings.push((binding.clone(), local));
                    }
                    self.emit_expr(&arm.body, destination);
                    for _ in bindings {
                        self.bindings.pop();
                    }
                    self.line("break;");
                    self.indent -= 1;
                    self.line("}");
                }
                self.line("default: slim_rt_trap(\"invalid variant tag\");");
                self.indent -= 1;
                self.line("}");
            }
            _ => unreachable!("checked match scrutinee"),
        }
    }

    fn evaluate(&mut self, expr: &Expr) -> String {
        let temporary = self.fresh_temp();
        self.line(&format!("{} {} = {{0}};", c_type(&expr.ty), temporary));
        self.emit_expr(expr, &temporary);
        temporary
    }

    fn assign_call(&mut self, destination: &str, function: &str, arguments: &[String]) {
        self.line(&format!(
            "{destination} = {function}({});",
            arguments.join(", ")
        ));
    }

    fn assign_binary(&mut self, destination: &str, arguments: &[String], operator: &str) {
        self.line(&format!(
            "{destination} = {} {operator} {};",
            arguments[0], arguments[1]
        ));
    }

    fn binding(&self, name: &str) -> String {
        self.bindings
            .iter()
            .rev()
            .find(|(source_name, _)| source_name == name)
            .map(|(_, c_name)| c_name.clone())
            .expect("checked binding")
    }

    fn fresh_temp(&mut self) -> String {
        let name = format!("slim_t_{}", self.next_temp);
        self.next_temp += 1;
        name
    }

    fn fresh_named(&mut self, name: &str) -> String {
        let unique = self.next_temp;
        self.next_temp += 1;
        format!("slim_v_{}_{}", mangle(name), unique)
    }

    fn line(&mut self, line: &str) {
        for _ in 0..self.indent {
            self.code.push_str("    ");
        }
        self.code.push_str(line);
        self.code.push('\n');
    }
}

fn c_type(ty: &Type) -> String {
    match ty {
        Type::Unit => "SlimUnit".to_owned(),
        Type::Bool => "bool".to_owned(),
        Type::U8 => "uint8_t".to_owned(),
        Type::I64 => "int64_t".to_owned(),
        Type::Bytes => "SlimBytes".to_owned(),
        Type::Vec(_) | Type::Arena(_) => "SlimVec".to_owned(),
        Type::Id(_) => "SlimId".to_owned(),
        Type::Named(name) => c_type_name(name),
        Type::Error => "SlimUnit".to_owned(),
    }
}

fn c_type_name(name: &str) -> String {
    format!("Slim_type_{}", mangle(name))
}

fn c_function_name(name: &str) -> String {
    format!("slim_fn_{}", mangle(name))
}

fn c_field_name(name: &str) -> String {
    format!("slim_field_{}", mangle(name))
}

fn c_case_name(name: &str) -> String {
    format!("slim_case_{}", mangle(name))
}

fn c_tag_name(variant: &str, case: &str) -> String {
    format!("SLIM_TAG_{}_{}", mangle(variant), mangle(case))
}

fn mangle(name: &str) -> String {
    let mut output = String::new();
    for byte in name.bytes() {
        if byte.is_ascii_alphanumeric() {
            output.push(char::from(byte));
        } else {
            write!(output, "_{byte:02X}").unwrap();
        }
    }
    output
}

fn c_byte_string(bytes: &[u8]) -> String {
    if bytes.is_empty() {
        return "\"\"".to_owned();
    }
    bytes
        .iter()
        .map(|byte| format!("\"\\x{byte:02x}\""))
        .collect::<Vec<_>>()
        .join("")
}

fn contains_recur(expr: &Expr) -> bool {
    match &expr.kind {
        ExprKind::Recur(_) => true,
        ExprKind::Let { value, body, .. } | ExprKind::Set { value, body, .. } => {
            contains_recur(value) || contains_recur(body)
        }
        ExprKind::Call { arguments, .. } => arguments.iter().any(contains_recur),
        ExprKind::Make { fields, .. } => fields.iter().any(|(_, value, _)| contains_recur(value)),
        ExprKind::Get { value, .. } => contains_recur(value),
        ExprKind::Case { payload, .. } => payload.iter().any(contains_recur),
        ExprKind::Match { value, arms } => {
            contains_recur(value) || arms.iter().any(|arm| contains_recur(&arm.body))
        }
        ExprKind::Unit
        | ExprKind::Bool(_)
        | ExprKind::I64(_)
        | ExprKind::Bytes(_)
        | ExprKind::Name(_)
        | ExprKind::Error => false,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{lexer, parser, sema, sexpr};

    fn compile(source: &str) -> String {
        let (tokens, lex_errors) = lexer::lex(source);
        assert!(lex_errors.is_empty(), "{lex_errors:#?}");
        let (forms, parse_errors) = sexpr::parse(&tokens, source.len());
        assert!(parse_errors.is_empty(), "{parse_errors:#?}");
        let (program, lower_errors) = parser::lower(&forms);
        assert!(lower_errors.is_empty(), "{lower_errors:#?}");
        let (checked, check_errors) = sema::check(program.unwrap());
        assert!(check_errors.is_empty(), "{check_errors:#?}");
        generate_c(&checked.unwrap())
    }

    #[test]
    fn emits_checked_arithmetic() {
        let c =
            compile("(module x (fn main ((args (Vec Bytes))) I64 (effects) (call i64.add 40 2)))");
        assert!(c.contains("slim_i64_add"));
        assert!(c.contains("slim_fn_main"));
    }

    #[test]
    fn emits_tail_recurrence_as_jump() {
        let c = compile(
            "(module x (fn countdown ((n I64)) I64 (effects partial) (match (call i64.eq n 0) (true 0) (false (recur (call i64.sub n 1))))) (fn main ((args (Vec Bytes))) I64 (effects partial) (call countdown 10)))",
        );
        assert!(c.contains("goto slim_recur"));
    }
}
