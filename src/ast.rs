use crate::span::Span;

#[derive(Clone, Debug)]
pub struct Program {
    pub name: String,
    pub items: Vec<Item>,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub enum Item {
    Function(Function),
    Record(Record),
    Variant(Variant),
}

#[derive(Clone, Debug)]
pub struct Function {
    pub name: String,
    pub params: Vec<Param>,
    pub return_type: Type,
    pub effects: Vec<Effect>,
    pub body: Expr,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct Param {
    pub name: String,
    pub ty: Type,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct Record {
    pub name: String,
    pub fields: Vec<Field>,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct Field {
    pub name: String,
    pub ty: Type,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct Variant {
    pub name: String,
    pub cases: Vec<VariantCase>,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub struct VariantCase {
    pub name: String,
    pub payload: Vec<Type>,
    pub span: Span,
}

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub enum Type {
    Unit,
    Bool,
    U8,
    I64,
    Bytes,
    Vec(Box<Type>),
    Arena(Box<Type>),
    Id(Box<Type>),
    Named(String),
    Error,
}

impl Type {
    pub fn display(&self) -> String {
        match self {
            Self::Unit => "Unit".to_owned(),
            Self::Bool => "Bool".to_owned(),
            Self::U8 => "U8".to_owned(),
            Self::I64 => "I64".to_owned(),
            Self::Bytes => "Bytes".to_owned(),
            Self::Vec(inner) => format!("(Vec {})", inner.display()),
            Self::Arena(inner) => format!("(Arena {})", inner.display()),
            Self::Id(inner) => format!("(Id {})", inner.display()),
            Self::Named(name) => name.clone(),
            Self::Error => "<error>".to_owned(),
        }
    }

    pub fn is_copy(&self) -> bool {
        matches!(
            self,
            Self::Unit | Self::Bool | Self::U8 | Self::I64 | Self::Id(_)
        )
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub enum Effect {
    Alloc,
    Io,
    Partial,
}

impl Effect {
    pub fn name(self) -> &'static str {
        match self {
            Self::Alloc => "alloc",
            Self::Io => "io",
            Self::Partial => "partial",
        }
    }
}

#[derive(Clone, Debug)]
pub struct Expr {
    pub kind: ExprKind,
    pub span: Span,
    pub ty: Type,
}

#[derive(Clone, Debug)]
pub enum ExprKind {
    Unit,
    Bool(bool),
    I64(i64),
    Bytes(Vec<u8>),
    Name(String),
    Let {
        name: String,
        declared_type: Type,
        value: Box<Expr>,
        body: Box<Expr>,
    },
    Call {
        function: String,
        arguments: Vec<Expr>,
    },
    Recur(Vec<Expr>),
    Set {
        name: String,
        value: Box<Expr>,
        body: Box<Expr>,
    },
    Make {
        record: String,
        fields: Vec<(String, Expr, Span)>,
    },
    Get {
        value: Box<Expr>,
        field: String,
    },
    Case {
        variant: String,
        case: String,
        payload: Vec<Expr>,
    },
    Match {
        value: Box<Expr>,
        arms: Vec<MatchArm>,
    },
    Error,
}

#[derive(Clone, Debug)]
pub struct MatchArm {
    pub pattern: Pattern,
    pub body: Expr,
    pub span: Span,
}

#[derive(Clone, Debug)]
pub enum Pattern {
    Bool(bool),
    Variant { case: String, bindings: Vec<String> },
}
