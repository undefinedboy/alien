
use serde::{Serialize, Deserialize};

pub type AstNode = Box<InnerNode>;

#[derive(Serialize, Deserialize, PartialEq, Debug)]
pub enum InnerNode {
    Program(Vec<AstNode>),
    Func { name: String, parameters: Vec<String>, body: AstNode },
    Var {
        name: String,
        #[serde(default)]
        initializer: Option<AstNode>
    },
    Const {
        name: String,
        initializer: AstNode
    },
    Block(Vec<AstNode>),
    If {
         condition: AstNode,
         then_branch: AstNode,
         #[serde(default)]
         else_branch: Option<AstNode>
    },
    While { condition: AstNode, body: AstNode },
    Return {
        #[serde(default)]
        value: Option<AstNode>
    },
    Exprstmt(AstNode),
    Assign { name: String, value: AstNode },
    Binary { left: AstNode, operator: String, right: AstNode },
    Logical {left: AstNode, operator: String, right: AstNode },
    Call { callee: AstNode, arguments: Vec<AstNode> },
    Grouping(AstNode),
    Unary { operator: String, right: AstNode },
    Variable { name: String },
    Number(f64),
    String(String),
    Literal(String),
}

pub fn build_ast(json: &str) -> Result<AstNode, serde_json::Error> {
    serde_json::from_str(json)
}