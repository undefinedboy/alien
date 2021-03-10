//
// Created by Alan Huang on 2/23/21.
//

#include "ast.h"

#include <cassert>

namespace alien {

void ClassDecl::trace(std::ostream &os) const {
  os << "class " << name.lexeme_ << " {\n";
  for (const auto& method : methods) {
    method->trace(os);
  }
  os << "}\n";
}

void FuncDecl::trace(std::ostream &os) const {
  os << "func " << name.lexeme_ << "(";
  if (!parameters.empty()) {
    os << parameters.front().lexeme_;
  }
  for (int i = 1; i < parameters.size(); i++) {
    os << ", " << parameters[i].lexeme_;
  }
  os << ")";
  os << " {\n";
  body->trace(os);
  os << "}\n";
}

void VarDecl::trace(std::ostream &os) const {
  os << "var " << name.lexeme_;
  if (initializer != nullptr) {
    os << " = ";
    initializer->trace(os);
  }
  os << ";";
}

void BlockStmt::trace(std::ostream &os) const {
  for (const auto& stmt : stmts) {
    stmt->trace(os);
    os << "\n";
  }
}

void IfStmt::trace(std::ostream &os) const {
  os << "if (";
  if (condition != nullptr) {
    condition->trace(os);
  }
  os << ") {\n";
  thenBranch->trace(os);
  os << "}";
  if (elseBranch != nullptr) {
    os << " else {\n";
    elseBranch->trace(os);
    os << "}";
  }
}

void WhileStmt::trace(std::ostream &os) const {
  os << "while (";
  if (condition != nullptr) {
    condition->trace(os);
  }
  os << ") {\n";
  body->trace(os);
  os << "}";
}

void ForStmt::trace(std::ostream &os) const {
  os << "for (";
  if (initializer != nullptr) {
    initializer->trace(os);
  }
  os << " ";
  if (condition != nullptr) {
    condition->trace(os);
  }
  os << "; ";
  if (increment != nullptr) {
    increment->trace(os);
  }
  os << ") {\n";
  body->trace(os);
  os << "}";
}

void PrintStmt::trace(std::ostream &os) const {
  os << "print ";
  if (expr != nullptr) {
    expr->trace(os);
  }
  os << ";";
}

void ReturnStmt::trace(std::ostream &os) const {
  os << "return";
  if (expr != nullptr) {
    os << " ";
    expr->trace(os);
  }
  os << ";";
}

void ExprStmt::trace(std::ostream &os) const {
  if (expr != nullptr) {
    expr->trace(os);
  }
  os << ";";
}

void Assign::trace(std::ostream &os) const {
  os << name.lexeme_ << " = ";
  value->trace(os);
}

void Binary::trace(std::ostream &os) const {
  left->trace(os);
  os <<  " " << op.lexeme_ << " ";
  right->trace(os);
}

void Call::trace(std::ostream &os) const {
  callee->trace(os);
  os << "(";
  if (!arguments.empty()) {
    arguments.front()->trace(os);
  }
  for (int i = 1; i < arguments.size(); i++) {
    os << ", ";
    arguments[i]->trace(os);
  }
  os << ")";
}

void Get::trace(std::ostream &os) const {
  object->trace(os);
  os << "." << name.lexeme_;
}

void Grouping::trace(std::ostream &os) const {
  os << "(";
  expr->trace(os);
  os << ")";
}

void Set::trace(std::ostream &os) const {
  object->trace(os);
  os << "." << name.lexeme_ << " = ";
  value->trace(os);
}

void Unary::trace(std::ostream &os) const {
  os << op.lexeme_;
  right->trace(os);
}

void Variable::trace(std::ostream &os) const {
  os << name.lexeme_;
}

void Logical::trace(std::ostream &os) const {
  left->trace(os);
  os << " " << op.lexeme_ << " ";
  right->trace(os);
}

void Number::trace(std::ostream &os) const {
  os << value;
}

void String::trace(std::ostream &os) const {
  os << str;
}

void Literal::trace(std::ostream &os) const {
  switch (literal) {
    case TOKEN_NIL: {
      os << "nil";
      break;
    }
    case TOKEN_FALSE: {
      os << "false";
      break;
    }
    case TOKEN_TRUE: {
      os << "true";
      break;
    }
    default:
      assert(false);
  }
}

void This::trace(std::ostream &os) const {
  os << "this";
}

}