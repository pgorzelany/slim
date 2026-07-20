use crate::span::{Source, Span};

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum Severity {
    Error,
    Warning,
}

impl Severity {
    fn as_str(self) -> &'static str {
        match self {
            Self::Error => "error",
            Self::Warning => "warning",
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Label {
    pub span: Span,
    pub message: String,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Fix {
    pub span: Span,
    pub replacement: String,
    pub message: String,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Diagnostic {
    pub code: &'static str,
    pub severity: Severity,
    pub message: String,
    pub primary: Span,
    pub labels: Vec<Label>,
    pub notes: Vec<String>,
    pub fixes: Vec<Fix>,
}

impl Diagnostic {
    pub fn error(code: &'static str, message: impl Into<String>, primary: Span) -> Self {
        Self {
            code,
            severity: Severity::Error,
            message: message.into(),
            primary,
            labels: Vec::new(),
            notes: Vec::new(),
            fixes: Vec::new(),
        }
    }

    pub fn label(mut self, span: Span, message: impl Into<String>) -> Self {
        self.labels.push(Label {
            span,
            message: message.into(),
        });
        self
    }

    pub fn note(mut self, note: impl Into<String>) -> Self {
        self.notes.push(note.into());
        self
    }

    pub fn fix(
        mut self,
        span: Span,
        replacement: impl Into<String>,
        message: impl Into<String>,
    ) -> Self {
        self.fixes.push(Fix {
            span,
            replacement: replacement.into(),
            message: message.into(),
        });
        self
    }

    pub fn render_human(&self, source: &Source) -> String {
        let (line, column) = source.line_column(self.primary.start);
        let line_text = source.line_text(line);
        let width = self
            .primary
            .end
            .saturating_sub(self.primary.start)
            .max(1)
            .min(line_text.len().saturating_sub(column - 1).max(1));
        let mut output = format!(
            "{}[{}]: {}\n  --> {}:{}:{}\n   |\n{:>3} | {}\n   | {}{}\n",
            self.severity.as_str(),
            self.code,
            self.message,
            source.path().display(),
            line,
            column,
            line,
            line_text,
            " ".repeat(column.saturating_sub(1)),
            "^".repeat(width),
        );
        for label in &self.labels {
            let (label_line, label_column) = source.line_column(label.span.start);
            output.push_str(&format!(
                "   = {}:{}: {}\n",
                label_line, label_column, label.message
            ));
        }
        for note in &self.notes {
            output.push_str(&format!("   = note: {note}\n"));
        }
        for fix in &self.fixes {
            let (fix_line, fix_column) = source.line_column(fix.span.start);
            output.push_str(&format!(
                "   = fix {}:{}: {} -> {:?}\n",
                fix_line, fix_column, fix.message, fix.replacement
            ));
        }
        output
    }

    pub fn render_json(&self, source: &Source) -> String {
        let (line, column) = source.line_column(self.primary.start);
        let labels = self
            .labels
            .iter()
            .map(|label| {
                format!(
                    "{{\"start\":{},\"end\":{},\"message\":\"{}\"}}",
                    label.span.start,
                    label.span.end,
                    escape_json(&label.message)
                )
            })
            .collect::<Vec<_>>()
            .join(",");
        let notes = self
            .notes
            .iter()
            .map(|note| format!("\"{}\"", escape_json(note)))
            .collect::<Vec<_>>()
            .join(",");
        let fixes = self
            .fixes
            .iter()
            .map(|fix| {
                format!(
                    "{{\"start\":{},\"end\":{},\"replacement\":\"{}\",\"message\":\"{}\"}}",
                    fix.span.start,
                    fix.span.end,
                    escape_json(&fix.replacement),
                    escape_json(&fix.message)
                )
            })
            .collect::<Vec<_>>()
            .join(",");
        format!(
            "{{\"code\":\"{}\",\"severity\":\"{}\",\"message\":\"{}\",\"file\":\"{}\",\"span\":{{\"start\":{},\"end\":{},\"line\":{},\"column\":{}}},\"labels\":[{}],\"notes\":[{}],\"fixes\":[{}]}}",
            self.code,
            self.severity.as_str(),
            escape_json(&self.message),
            escape_json(&source.path().display().to_string()),
            self.primary.start,
            self.primary.end,
            line,
            column,
            labels,
            notes,
            fixes,
        )
    }
}

fn escape_json(value: &str) -> String {
    let mut escaped = String::with_capacity(value.len());
    for ch in value.chars() {
        match ch {
            '"' => escaped.push_str("\\\""),
            '\\' => escaped.push_str("\\\\"),
            '\n' => escaped.push_str("\\n"),
            '\r' => escaped.push_str("\\r"),
            '\t' => escaped.push_str("\\t"),
            ch if ch < ' ' => escaped.push_str(&format!("\\u{:04x}", ch as u32)),
            ch => escaped.push(ch),
        }
    }
    escaped
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    #[test]
    fn renders_location_in_both_formats() {
        let source = Source::new(PathBuf::from("test.slim"), "one\ntwo\n".to_owned());
        let diagnostic = Diagnostic::error("E0001", "problem", Span::new(4, 7));
        assert!(diagnostic.render_human(&source).contains("test.slim:2:1"));
        assert!(diagnostic.render_json(&source).contains("\"line\":2"));
    }
}
