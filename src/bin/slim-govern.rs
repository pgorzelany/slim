use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};

use slim::sema::Builtin;

const REQUIRED_HEADINGS: [&str; 5] = [
    "## Need",
    "## Alternatives",
    "## Costs",
    "## Evidence",
    "## Removal",
];

#[derive(Debug)]
struct Decision {
    id: String,
    status: String,
    kind: String,
    primitive: String,
    ratings: [i32; 6],
    score: i32,
}

fn main() {
    let command = env::args().nth(1).unwrap_or_else(|| "check".to_owned());
    if command != "check" {
        eprintln!("usage: slim-govern check");
        std::process::exit(64);
    }

    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let errors = check_repository(&root);
    if errors.is_empty() {
        println!("governance: all hard gates passed");
        return;
    }

    for error in &errors {
        eprintln!("governance error: {error}");
    }
    eprintln!("governance: {} hard gate(s) failed", errors.len());
    std::process::exit(1);
}

fn check_repository(root: &Path) -> Vec<String> {
    let mut errors = Vec::new();
    let required = [
        "AGENTS.md",
        "DESIGN.md",
        "docs/CORE.md",
        "design/FEATURE_POLICY.md",
        "design/rust-budget.tsv",
        "design/surface.tsv",
    ];
    for path in required {
        if !root.join(path).is_file() {
            errors.push(format!("missing required policy file {path}"));
        }
    }

    let decisions = load_decisions(&root.join("design/decisions"), &mut errors);
    check_decisions(&decisions, &mut errors);
    check_surface(&root.join("design/surface.tsv"), &decisions, &mut errors);
    check_conformance_coverage(root, &mut errors);
    check_dependencies(&root.join("Cargo.toml"), &mut errors);
    check_rust_safety(&root.join("src"), &mut errors);
    check_rust_budget(root, &mut errors);
    errors
}

fn check_conformance_coverage(root: &Path, errors: &mut Vec<String>) {
    let surface_path = root.join("design/surface.tsv");
    let manifest_path = root.join("conformance/manifest.tsv");
    let Ok(surface) = fs::read_to_string(&surface_path) else {
        return;
    };
    let manifest = match fs::read_to_string(&manifest_path) {
        Ok(manifest) => manifest,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", manifest_path.display()));
            return;
        }
    };
    let required: BTreeSet<_> = surface
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter_map(|line| {
            let columns: Vec<_> = line.split('\t').collect();
            (columns.len() == 4).then(|| format!("{}:{}", columns[0], columns[1]))
        })
        .collect();
    let covered: BTreeSet<_> = manifest
        .lines()
        .filter(|line| !line.is_empty() && !line.starts_with('#'))
        .filter_map(|line| line.split('\t').nth(5))
        .flat_map(|tags| tags.split(','))
        .map(str::to_owned)
        .collect();
    for missing in required.difference(&covered) {
        errors.push(format!(
            "accepted surface `{missing}` has no conformance coverage tag"
        ));
    }
}

fn load_decisions(dir: &Path, errors: &mut Vec<String>) -> BTreeMap<String, Decision> {
    let mut decisions = BTreeMap::new();
    let entries = match fs::read_dir(dir) {
        Ok(entries) => entries,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", dir.display()));
            return decisions;
        }
    };

    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|value| value.to_str()) != Some("md") {
            continue;
        }
        let file_name = path
            .file_name()
            .and_then(|value| value.to_str())
            .unwrap_or("");
        let Some(id) = file_name.split('-').next() else {
            continue;
        };
        if id.len() != 5 || !id.starts_with('D') || !id[1..].chars().all(|ch| ch.is_ascii_digit()) {
            errors.push(format!("invalid decision filename {file_name}"));
            continue;
        }

        let text = match fs::read_to_string(&path) {
            Ok(text) => text,
            Err(error) => {
                errors.push(format!("cannot read {}: {error}", path.display()));
                continue;
            }
        };
        for heading in REQUIRED_HEADINGS {
            if !text.lines().any(|line| line.trim() == heading) {
                errors.push(format!("{file_name} is missing {heading}"));
            }
        }

        let fields = parse_fields(&text);
        let required_field = |name: &str, errors: &mut Vec<String>| -> String {
            match fields.get(name) {
                Some(value) => value.clone(),
                None => {
                    errors.push(format!("{file_name} is missing {name}:"));
                    String::new()
                }
            }
        };
        let status = required_field("Status", errors);
        let kind = required_field("Kind", errors);
        let primitive = required_field("Primitive", errors);
        let mut ratings = [0; 6];
        for (index, name) in [
            "Safety", "Compile", "Runtime", "Minimal", "Analysis", "Dogfood",
        ]
        .iter()
        .enumerate()
        {
            let value = required_field(name, errors);
            ratings[index] = parse_number(&value, file_name, name, errors);
            if !(-2..=2).contains(&ratings[index]) {
                errors.push(format!(
                    "{file_name} {name} rating must be between -2 and 2"
                ));
            }
        }
        let score_text = required_field("Score", errors);
        let score = parse_number(&score_text, file_name, "Score", errors);

        if decisions
            .insert(
                id.to_owned(),
                Decision {
                    id: id.to_owned(),
                    status,
                    kind,
                    primitive,
                    ratings,
                    score,
                },
            )
            .is_some()
        {
            errors.push(format!("duplicate decision id {id}"));
        }
    }
    decisions
}

fn parse_fields(text: &str) -> BTreeMap<String, String> {
    text.lines()
        .filter_map(|line| {
            let (key, value) = line.split_once(':')?;
            if key.chars().all(|ch| ch.is_ascii_alphabetic()) {
                Some((key.to_owned(), value.trim().to_owned()))
            } else {
                None
            }
        })
        .collect()
}

fn parse_number(value: &str, file: &str, field: &str, errors: &mut Vec<String>) -> i32 {
    value.parse().unwrap_or_else(|_| {
        errors.push(format!("{file} has invalid integer for {field}: {value}"));
        0
    })
}

fn check_decisions(decisions: &BTreeMap<String, Decision>, errors: &mut Vec<String>) {
    let mut primitives = BTreeSet::new();
    for decision in decisions.values() {
        let weights = [20, 20, 20, 20, 15, 5];
        let weighted: i32 = decision
            .ratings
            .iter()
            .zip(weights)
            .map(|(rating, weight)| rating * weight)
            .sum();
        if weighted % 2 != 0 {
            errors.push(format!(
                "{} score is fractional; choose ratings whose weighted sum is even",
                decision.id
            ));
        }
        let calculated = weighted / 2;
        if decision.score != calculated {
            errors.push(format!(
                "{} declares score {} but calculated score is {}",
                decision.id, decision.score, calculated
            ));
        }
        if decision.status == "accepted" && decision.kind == "language" {
            if decision.score < 40 {
                errors.push(format!(
                    "{} accepted language score is below 40",
                    decision.id
                ));
            }
            if decision.ratings[..4].iter().any(|rating| *rating < 0) {
                errors.push(format!(
                    "{} has a negative hard-gate rating in safety/compile/runtime/minimal",
                    decision.id
                ));
            }
            if !decision.ratings.contains(&2) {
                errors.push(format!("{} has no primary +2 benefit", decision.id));
            }
        }
        if decision.primitive != "none" && !primitives.insert(decision.primitive.clone()) {
            errors.push(format!(
                "{} duplicates accepted primitive {}",
                decision.id, decision.primitive
            ));
        }
    }
}

fn check_surface(path: &Path, decisions: &BTreeMap<String, Decision>, errors: &mut Vec<String>) {
    let text = match fs::read_to_string(path) {
        Ok(text) => text,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", path.display()));
            return;
        }
    };
    let mut names = BTreeSet::new();
    let mut roles = BTreeSet::new();
    let mut builtin_names = BTreeSet::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 4 {
            errors.push(format!(
                "surface.tsv:{} must have four columns",
                line_index + 1
            ));
            continue;
        }
        let category_name = format!("{}:{}", columns[0], columns[1]);
        if !names.insert(category_name.clone()) {
            errors.push(format!("duplicate surface name {category_name}"));
        }
        let category_role = format!("{}:{}", columns[0], columns[2]);
        if !roles.insert(category_role.clone()) {
            errors.push(format!("duplicate semantic role {category_role}"));
        }
        match decisions.get(columns[3]) {
            Some(decision) if decision.status == "accepted" => {}
            Some(_) => errors.push(format!(
                "surface {category_name} cites unaccepted {}",
                columns[3]
            )),
            None => errors.push(format!(
                "surface {category_name} cites missing {}",
                columns[3]
            )),
        }
        if columns[0] == "builtin" {
            builtin_names.insert(columns[1].to_owned());
        }
    }
    let implemented: BTreeSet<_> = Builtin::all()
        .iter()
        .map(|builtin| builtin.name().to_owned())
        .collect();
    for missing in implemented.difference(&builtin_names) {
        errors.push(format!(
            "implemented built-in `{missing}` is missing from surface.tsv"
        ));
    }
    for missing in builtin_names.difference(&implemented) {
        errors.push(format!(
            "surface built-in `{missing}` has no implementation"
        ));
    }
}

fn check_dependencies(path: &Path, errors: &mut Vec<String>) {
    let Ok(text) = fs::read_to_string(path) else {
        return;
    };
    let mut in_dependencies = false;
    for line in text.lines() {
        let trimmed = line.trim();
        if trimmed.starts_with('[') {
            in_dependencies = matches!(
                trimmed,
                "[dependencies]" | "[build-dependencies]" | "[dev-dependencies]"
            );
            continue;
        }
        if in_dependencies && !trimmed.is_empty() && !trimmed.starts_with('#') {
            errors.push(format!("unapproved Cargo dependency: {trimmed}"));
        }
    }
}

fn check_rust_safety(dir: &Path, errors: &mut Vec<String>) {
    let unsafe_block = ["un", "safe {"].concat();
    let unsafe_function = ["un", "safe fn"].concat();
    visit_rs_files(dir, &mut |path, text| {
        if text.contains(&unsafe_block) || text.contains(&unsafe_function) {
            errors.push(format!(
                "unsafe Rust requires a dedicated accepted decision: {}",
                path.display()
            ));
        }
    });
}

fn check_rust_budget(root: &Path, errors: &mut Vec<String>) {
    let path = root.join("design/rust-budget.tsv");
    let text = match fs::read_to_string(&path) {
        Ok(text) => text,
        Err(error) => {
            errors.push(format!("cannot read {}: {error}", path.display()));
            return;
        }
    };
    let mut budget = BTreeMap::<String, (String, Option<usize>)>::new();
    for (line_index, line) in text.lines().enumerate() {
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let columns: Vec<_> = line.split('\t').collect();
        if columns.len() != 3 {
            errors.push(format!(
                "rust-budget.tsv:{} must have three tab-separated columns",
                line_index + 1
            ));
            continue;
        }
        let maximum = match columns[1] {
            "production" => match columns[2].parse::<usize>() {
                Ok(maximum) => Some(maximum),
                Err(_) => {
                    errors.push(format!(
                        "rust-budget.tsv:{} production maximum must be bytes",
                        line_index + 1
                    ));
                    continue;
                }
            },
            "infrastructure" if columns[2] == "-" => None,
            "infrastructure" => {
                errors.push(format!(
                    "rust-budget.tsv:{} infrastructure maximum must be -",
                    line_index + 1
                ));
                continue;
            }
            role => {
                errors.push(format!(
                    "rust-budget.tsv:{} unknown Rust role {role}",
                    line_index + 1
                ));
                continue;
            }
        };
        if budget
            .insert(columns[0].to_owned(), (columns[1].to_owned(), maximum))
            .is_some()
        {
            errors.push(format!("duplicate Rust budget path {}", columns[0]));
        }
    }

    let mut seen = BTreeSet::new();
    visit_rs_files(&root.join("src"), &mut |path, source| {
        let relative = path
            .strip_prefix(root)
            .unwrap_or(path)
            .to_string_lossy()
            .replace('\\', "/");
        seen.insert(relative.clone());
        match budget.get(&relative) {
            Some((role, Some(maximum))) if source.len() > *maximum => errors.push(format!(
                "production Rust budget exceeded for {relative}: {} > {maximum} bytes; implement capability in SLIM or accept a new architecture decision",
                source.len()
            )),
            Some((role, None)) if role == "infrastructure" => {}
            Some(_) => {}
            None => errors.push(format!(
                "unclassified Rust source {relative}; add it to rust-budget.tsv"
            )),
        }
    });
    for missing in budget.keys().filter(|path| !seen.contains(*path)) {
        errors.push(format!("Rust budget path does not exist: {missing}"));
    }
}

fn visit_rs_files(dir: &Path, action: &mut impl FnMut(&Path, &str)) {
    let Ok(entries) = fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            visit_rs_files(&path, action);
        } else if path.extension().and_then(|value| value.to_str()) == Some("rs")
            && let Ok(text) = fs::read_to_string(&path)
        {
            action(&path, &text);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn repository_passes_governance() {
        let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        let errors = check_repository(&root);
        assert!(errors.is_empty(), "{}", errors.join("\n"));
    }

    #[test]
    fn production_rust_growth_fails_governance() {
        let root = std::env::temp_dir().join(format!("slim-govern-budget-{}", std::process::id()));
        let _ = fs::remove_dir_all(&root);
        fs::create_dir_all(root.join("src")).unwrap();
        fs::create_dir_all(root.join("design")).unwrap();
        fs::write(root.join("src/feature.rs"), "fn feature() {}\n").unwrap();
        fs::write(
            root.join("design/rust-budget.tsv"),
            "src/feature.rs\tproduction\t1\n",
        )
        .unwrap();
        let mut errors = Vec::new();
        check_rust_budget(&root, &mut errors);
        assert!(
            errors
                .iter()
                .any(|error| error.contains("production Rust budget exceeded"))
        );
        fs::remove_dir_all(root).unwrap();
    }
}
