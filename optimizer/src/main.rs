mod ast;

use std::fs;
use ast::ast_builder::{AstNode, build_ast};

fn main() {
    let json = fs::read_to_string("../test/testjsongen.json").unwrap();
    let ast: AstNode = build_ast(&json).unwrap();
    println!("{:?}", ast);
}