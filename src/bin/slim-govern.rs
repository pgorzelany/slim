use std::collections::{BTreeMap, BTreeSet};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};

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
    check_dependencies(&root.join("Cargo.toml"), &mut errors);
    check_rust_safety(&root.join("src"), &mut errors);
    errors
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
}
