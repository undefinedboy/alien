#ifndef ALIEN_JSONGEN_H
#define ALIEN_JSONGEN_H

#include <ast.h>
#include <typedef.h>

#include <fstream>
#include <vector>
#include <stack>

#include <json.h>

namespace alien {

class JsonGenerator : public StmtVisitor, public ExprVisitor {
public:
  void generate_json(std::ostream& os, const std::vector<StmtPtr>& program);
  void visit(ClassDecl &decl) override;
  void visit(FuncDecl &decl) override;
  void visit(VarDecl &decl) override;
  void visit(BlockStmt &stmts) override;
  void visit(IfStmt &stmt) override;
  void visit(WhileStmt &stmt) override;
  void visit(ForStmt &stmt) override;
  void visit(PrintStmt &stmt) override;
  void visit(ReturnStmt &stmt) override;
  void visit(ExprStmt&stmt) override;
  void visit(Assign& expr) override;
  void visit(Binary& expr) override;
  void visit(Call& expr) override;
  void visit(Get& expr) override;
  void visit(Grouping& expr) override;
  void visit(Set& expr) override;
  void visit(Unary& expr) override;
  void visit(Variable& expr) override;
  void visit(Logical& expr) override;
  void visit(Number& expr) override;
  void visit(String& expr) override;
  void visit(Literal& expr) override;
  void visit(This& expr) override;
  
private:
  void push() { jLevel_.push(jCurrent_); }
  void pop() { jCurrent_ = jLevel_.top(); jLevel_.pop(); }

private:
  nlohmann::json json_;
  nlohmann::json* jCurrent_;
  std::stack<nlohmann::json*> jLevel_;
};

} // namespace alien

#endif