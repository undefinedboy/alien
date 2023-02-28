//
// Created by Alan Huang on 2/27/21.
//

#ifndef ALIEN_COMPILER_H
#define ALIEN_COMPILER_H

#include <ast.h>
#include <vm.h>
#include <chunk.h>
#include <object.h>

#include <string_view>
#include <unordered_map>

namespace alien {

class Compiler : public StmtVisitor, public ExprVisitor {
public:
  explicit Compiler(Vm& vm)
  : vm_(vm), currentChunk_(&globalChunk_) {}
  ObjFunction* compile(std::vector<StmtPtr>& stmts);
  void visit(ClassDecl &decl) override;
  void visit(FuncDecl &decl) override;
  void visit(VarDecl &decl) override;
  void visit(ConstDecl &decl) override;
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
  bool hadError() { return hadError_; }
private:
  void fixJump(int offset);
  void emitLoop(int loopStart);

private:
  struct Local {
    int depth;
    std::string_view name;
  };
  void addLocal(std::string_view name);
  int  resolveLocal(std::string_view name);
  void beginScope();
  void endScope();
  void initFunction(std::string_view);

private:
  // we use this to clarify whether
  // a variable declaration is in local scope.
  ObjClass* currentClass_ = nullptr;
  bool isInitializer = false;
  bool hadError_ = false;
  int depth_ = 0;
  Vm& vm_;
  Chunk globalChunk_;
  // this chunk belongs to the function being compiled.
  Chunk* currentChunk_;

  // the first slot is for `this` or the function's name.
  // remember to clear it when begin to compile a function or method.
  std::vector<Local> locals_;
};

}

#endif //ALIEN_COMPILER_H
