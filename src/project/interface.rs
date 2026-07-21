use std::collections::{BTreeMap, BTreeSet};
use std::fmt::Write as _;

use crate::ast::{Effect, Item, ParamMode, Type};
use crate::incremental::Fingerprint;

use super::{LoadedProject, ProjectDiagnostic};

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct InterfaceArtifact {
    pub module: String,
    pub bytes: String,
    pub fingerprint: Fingerprint,
}

pub fn build(
    project: &LoadedProject,
) -> Result<BTreeMap<String, InterfaceArtifact>, Vec<ProjectDiagnostic>> {
    let mut artifacts = BTreeMap::new();
    let mut diagnostics = Vec::new();
    for spec in &project.manifest.modules {
        let module = project
            .modules
            .get(&spec.identity.value)
            .expect("loaded project contains every manifest module");
        let by_name: BTreeMap<_, _> = module
            .program
            .items
            .iter()
            .map(|item| (item_name(item), item))
            .collect();
        let mut declarations = Vec::new();
        for export in &spec.exports {
            let Some(item) = by_name.get(export.value.as_str()).copied() else {
                continue;
            };
            if let Some(encoded) = encode_item(
                project,
                &spec.identity.value,
                item,
                export.span,
                &mut diagnostics,
            ) {
                declarations.push(encoded);
            }
        }
        let mut bytes = format!("(interface 1 {}", spec.identity.value);
        for declaration in declarations {
            bytes.push(' ');
            bytes.push_str(&declaration);
        }
        bytes.push_str(")\n");
        artifacts.insert(
            spec.identity.value.clone(),
            InterfaceArtifact {
                module: spec.identity.value.clone(),
                fingerprint: stable_fingerprint(bytes.as_bytes()),
                bytes,
            },
        );
    }
    if diagnostics.is_empty() {
        Ok(artifacts)
    } else {
        diagnostics.sort_by(|left, right| {
            left.module
                .cmp(&right.module)
                .then_with(|| {
                    left.diagnostic
                        .primary
                        .start
                        .cmp(&right.diagnostic.primary.start)
                })
                .then_with(|| left.diagnostic.code.cmp(right.diagnostic.code))
        });
        Err(diagnostics)
    }
}

fn encode_item(
    project: &LoadedProject,
    module: &str,
    item: &Item,
    export_span: crate::span::Span,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) -> Option<String> {
    let mut output = String::new();
    let before = diagnostics.len();
    match item {
        Item::Function(function) => {
            write!(output, "(fn {} (", function.name).unwrap();
            for (index, parameter) in function.params.iter().enumerate() {
                if index > 0 {
                    output.push(' ');
                }
                let ty = encode_type(project, module, &parameter.ty, export_span, diagnostics);
                write!(
                    output,
                    "({} {ty})",
                    match parameter.mode {
                        ParamMode::Owned => "owned",
                        ParamMode::Inout => "inout",
                    }
                )
                .unwrap();
            }
            let return_type = encode_type(
                project,
                module,
                &function.return_type,
                export_span,
                diagnostics,
            );
            write!(output, ") {return_type} (effects").unwrap();
            let effects: BTreeSet<_> = function.effects.iter().copied().collect();
            for effect in [Effect::Alloc, Effect::Io, Effect::Partial] {
                if effects.contains(&effect) {
                    write!(output, " {}", effect.name()).unwrap();
                }
            }
            output.push_str("))");
        }
        Item::Record(record) => {
            write!(output, "(record {} (", record.name).unwrap();
            for (index, field) in record.fields.iter().enumerate() {
                if index > 0 {
                    output.push(' ');
                }
                let ty = encode_type(project, module, &field.ty, export_span, diagnostics);
                write!(output, "({} {ty})", field.name).unwrap();
            }
            output.push_str("))");
        }
        Item::Variant(variant) => {
            write!(output, "(variant {} (", variant.name).unwrap();
            for (case_index, case) in variant.cases.iter().enumerate() {
                if case_index > 0 {
                    output.push(' ');
                }
                write!(output, "({}", case.name).unwrap();
                for ty in &case.payload {
                    let ty = encode_type(project, module, ty, export_span, diagnostics);
                    write!(output, " {ty}").unwrap();
                }
                output.push(')');
            }
            output.push_str("))");
        }
    }
    (diagnostics.len() == before).then_some(output)
}

fn encode_type(
    project: &LoadedProject,
    module: &str,
    ty: &Type,
    export_span: crate::span::Span,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) -> String {
    match ty {
        Type::Unit => "Unit".to_owned(),
        Type::Bool => "Bool".to_owned(),
        Type::U8 => "U8".to_owned(),
        Type::I64 => "I64".to_owned(),
        Type::Bytes => "Bytes".to_owned(),
        Type::Vec(inner) => format!(
            "(Vec {})",
            encode_type(project, module, inner, export_span, diagnostics)
        ),
        Type::Arena(inner) => format!(
            "(Arena {})",
            encode_type(project, module, inner, export_span, diagnostics)
        ),
        Type::Id(inner) => format!(
            "(Id {})",
            encode_type(project, module, inner, export_span, diagnostics)
        ),
        Type::Named(name) => qualify_named_type(project, module, name, export_span, diagnostics),
        Type::Error => "<error>".to_owned(),
    }
}

fn qualify_named_type(
    project: &LoadedProject,
    module: &str,
    name: &str,
    export_span: crate::span::Span,
    diagnostics: &mut Vec<ProjectDiagnostic>,
) -> String {
    let current = project
        .modules
        .get(module)
        .expect("current module is loaded");
    let (owner, declaration) = if let Some((owner, declaration)) = name.split_once('/') {
        if owner == module {
            diagnostics.push(ProjectDiagnostic::local(
                Some(module.to_owned()),
                project.manifest_source.clone(),
                crate::diagnostic::Diagnostic::error(
                    "E0417",
                    format!("module `{module}` must not qualify its own type `{name}`"),
                    export_span,
                ),
            ));
            return name.to_owned();
        }
        if !current
            .spec
            .imports
            .iter()
            .any(|import| import.value == owner)
        {
            diagnostics.push(ProjectDiagnostic::local(
                Some(module.to_owned()),
                project.manifest_source.clone(),
                crate::diagnostic::Diagnostic::error(
                    "E0416",
                    format!("interface uses module `{owner}` without a direct import"),
                    export_span,
                ),
            ));
            return name.to_owned();
        }
        (owner, declaration)
    } else {
        (module, name)
    };
    let owner_module = project
        .modules
        .get(owner)
        .expect("imported module is loaded");
    let Some(item) = owner_module
        .program
        .items
        .iter()
        .find(|item| item_name(item) == declaration)
    else {
        diagnostics.push(ProjectDiagnostic::local(
            Some(module.to_owned()),
            project.manifest_source.clone(),
            crate::diagnostic::Diagnostic::error(
                "E0418",
                format!("interface references absent type `{owner}/{declaration}`"),
                export_span,
            ),
        ));
        return format!("{owner}/{declaration}");
    };
    if matches!(item, Item::Function(_)) {
        diagnostics.push(ProjectDiagnostic::local(
            Some(module.to_owned()),
            project.manifest_source.clone(),
            crate::diagnostic::Diagnostic::error(
                "E0418",
                format!("interface name `{owner}/{declaration}` is not a type"),
                export_span,
            ),
        ));
    }
    if !owner_module
        .spec
        .exports
        .iter()
        .any(|export| export.value == declaration)
    {
        diagnostics.push(ProjectDiagnostic::local(
            Some(module.to_owned()),
            project.manifest_source.clone(),
            crate::diagnostic::Diagnostic::error(
                "E0418",
                format!("exported interface leaks private type `{owner}/{declaration}`"),
                export_span,
            ),
        ));
    }
    format!("{owner}/{declaration}")
}

fn item_name(item: &Item) -> &str {
    match item {
        Item::Function(function) => &function.name,
        Item::Record(record) => &record.name,
        Item::Variant(variant) => &variant.name,
    }
}

fn stable_fingerprint(bytes: &[u8]) -> Fingerprint {
    let mut value = 0xcbf29ce484222325_u64;
    for byte in bytes {
        value ^= u64::from(*byte);
        value = value.wrapping_mul(0x100000001b3);
    }
    Fingerprint(value)
}

#[cfg(test)]
mod tests {
    use std::fs;

    use crate::span::Source;

    use super::*;

    #[test]
    fn canonical_interface_ignores_private_body_and_paths() {
        let root = std::env::temp_dir().join(format!("slim-interface-test-{}", std::process::id()));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir(&root).unwrap();
        let manifest = "(project 1 (entry app) (module app \"app.slim\" (imports math) (exports)) (module math \"math.slim\" (imports) (exports Number add)))";
        fs::write(
            root.join("app.slim"),
            "(module app (fn main ((args (Vec Bytes))) I64 (effects) 0))",
        )
        .unwrap();
        fs::write(
            root.join("math.slim"),
            "(module math (record Number ((value I64))) (fn add ((left Number) (right Number)) Number (effects) left))",
        )
        .unwrap();
        let loaded =
            super::super::load(Source::new(root.join("slim.project"), manifest.to_owned()))
                .unwrap();
        let first = build(&loaded).unwrap();
        let math = &first["math"];
        assert_eq!(
            math.bytes,
            "(interface 1 math (record Number ((value I64))) (fn add ((owned math/Number) (owned math/Number)) math/Number (effects)))\n"
        );

        fs::write(
            root.join("math.slim"),
            "(module math (record Number ((value I64))) (fn add ((left Number) (right Number)) Number (effects) right))",
        )
        .unwrap();
        let loaded =
            super::super::load(Source::new(root.join("slim.project"), manifest.to_owned()))
                .unwrap();
        assert_eq!(first, build(&loaded).unwrap());
        fs::remove_dir_all(root).unwrap();
    }
}
