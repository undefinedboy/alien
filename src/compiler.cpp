//
// Created by Alan Huang on 2/27/21.
//

#include <compiler.h>
#include <vm.h>
#include <ast.h>
#include <value.h>
#include <object.h>

#include <vector>
#include <string_view>

#include <cassert>

namespace alien {

namespace {
  // TODO
  void compileTimeError(std::string_view message) {
    std::cerr << message << '\n';
  }
} // namespace

void Compiler::fixJump(int offset) {
  assert(currentChunk_->code()[offset] == 0xff);
  // -1 to skip the offset itself.
  auto code = static_cast<OpCode>(currentChunk_->code().size() - offset - 1);
  currentChunk_->code()[offset] = code;
}

void Compiler::emitLoop(int loopStart) {
  // +1 to skip the loop's offset.
  currentChunk_->write(OP_LOOP);
  int offset = currentChunk_->code().size() + 1 - loopStart;
  currentChunk_->write(static_cast<OpCode>(offset));
}

void Compiler::addLocal(std::string_view name) {
  locals_.push_back(Local{depth_, name});
}

// TODO: check this `var a = a;`
int Compiler::resolveLocal(std::string_view name) {
  for (int i = locals_.size() - 1; i >= 0; i--) {
    if (locals_[i].name == name) {
      return i;
    }
  }
  return -1;
}

void Compiler::beginScope() {
  depth_++;
}

void Compiler::endScope() {
  for (int i = locals_.size() - 1; i >= 0; i--) {
    if (locals_[i].depth == depth_) {
      locals_.pop_back();
      // for statement blocks.
      currentChunk_->write(OP_POP);
    }
  }
  depth_--;
}

void Compiler::initFunction(std::string_view name) {
  locals_.clear();
  // if we set the first slot to `this`, we couldn't recursively call the method.
  // that's bullshit because we have `this` and we should call method by `this`.
  // can't set the slot to `init` cause we still need `this` in initializer.
  addLocal(currentClass_ ? "this" : name);
  isInitializer = currentClass_ && name == "init";
}

ObjFunction* Compiler::compile(std::vector<StmtPtr>& stmts) {
  for (const auto& stmt : stmts) {
    stmt->accept(*this);
  }
  globalChunk_.write(OP_GET_GLOBAL);
  std::string name("main");
  int index = globalChunk_.addConstant(name);
  globalChunk_.write(static_cast<OpCode>(index));
  globalChunk_.write(OP_CALL);
  globalChunk_.write(static_cast<OpCode>(0));
  globalChunk_.write(OP_NIL);
  globalChunk_.write(OP_RETURN);
  return new ObjFunction("script", globalChunk_, 0);
}

void Compiler::visit(ClassDecl &decl) {
  std::string name(decl.name.lexeme_.data(), decl.name.lexeme_.size());
  currentClass_ = new ObjClass(name);
  vm_.addObj(currentClass_);
  for (const auto& method : decl.methods) {
    method->accept(*this);
  }
  // TODO: attention the implicit conversion
  int index = globalChunk_.addConstant(currentClass_);
  globalChunk_.write(OP_CONSTANT);
  globalChunk_.write(static_cast<OpCode>(index));
  globalChunk_.write(OP_DEFINE_GLOBAL);
  index = globalChunk_.addConstant(name);
  globalChunk_.write(static_cast<OpCode>(index));
  currentClass_ = nullptr;
}

void Compiler::visit(FuncDecl &decl) {
  // let the variable declaration in this function
  // not to be in the global.
  beginScope();
  Chunk chunk;
  currentChunk_ = &chunk;
  initFunction(decl.name.lexeme_);
  // TODO: check whether the parameter is duplicated.
  for (const auto& parameter : decl.parameters) {
    addLocal(parameter.lexeme_);
  }
  // we want the arguments and the ObjFunction to be in the same scope.
  auto blockStmt = dynamic_cast<BlockStmt*>(decl.body.get());
  for (const auto& stmt : blockStmt->stmts) {
    stmt->accept(*this);
  }
  if (decl.name.lexeme_ == "init" && currentClass_) {
    currentChunk_->write(OP_GET_LOCAL);
    currentChunk_->write(static_cast<OpCode>(0));
  } else {
    currentChunk_->write(OP_NIL);
  }
  currentChunk_->write(OP_RETURN);
  std::string name(decl.name.lexeme_.data(), decl.name.lexeme_.size());
  auto func = new ObjFunction(name, chunk, decl.parameters.size());
  // for garbage collection.
  vm_.addObj(func);
  if (currentClass_) {
    // this is a method.
    currentClass_->addMethod(name, func);
  } else {
    int index = globalChunk_.addConstant(func);
    globalChunk_.write(OP_CONSTANT);
    globalChunk_.write(static_cast<OpCode>(index));
    index = globalChunk_.addConstant(name);
    globalChunk_.write(OP_DEFINE_GLOBAL);
    globalChunk_.write(static_cast<OpCode>(index));
  }
  // we don't generate a series of OP_POP;
  depth_--;
  currentChunk_ = &globalChunk_;
}

void Compiler::visit(VarDecl &decl) {
  if (depth_ == 0) {
    std::string name(decl.name.lexeme_.data(), decl.name.lexeme_.size());
    if (decl.initializer) {
      decl.initializer->accept(*this);
    } else  {
      globalChunk_.write(OP_NIL);
    }
    globalChunk_.write(OP_DEFINE_GLOBAL);
    int index = currentChunk_->addConstant(name);
    currentChunk_->write(static_cast<OpCode>(index));
  } else {
    if (decl.initializer) {
      decl.initializer->accept(*this);
    } else {
      currentChunk_->write(OP_NIL);
    }
    addLocal(decl.name.lexeme_);
  }
}

void Compiler::visit(BlockStmt &stmts) {
  beginScope();
  for (const auto& stmt : stmts.stmts) {
    stmt->accept(*this);
  }
  endScope();
}

void Compiler::visit(IfStmt &stmt) {
  stmt.condition->accept(*this);
  currentChunk_->write(OP_JUMP_IF_FALSE);
  int thenJump = currentChunk_->code().size();
  // sentinel
  currentChunk_->write(static_cast<OpCode>(0xff));
  // pop the condition.
  currentChunk_->write(OP_POP);
  stmt.thenBranch->accept(*this);
  currentChunk_->write(OP_JUMP);
  int elseJump = currentChunk_->code().size();
  currentChunk_->write(static_cast<OpCode>(0xff));
  fixJump(thenJump);
  currentChunk_->write(OP_POP);
  if (stmt.elseBranch) {
    stmt.elseBranch->accept(*this);
  }
  fixJump(elseJump);
}

void Compiler::visit(WhileStmt &stmt) {
  int loopStart = currentChunk_->code().size();
  stmt.condition->accept(*this);
  currentChunk_->write(OP_JUMP_IF_FALSE);
  int jump = currentChunk_->code().size();
  currentChunk_->write(static_cast<OpCode>(0xff));
  currentChunk_->write(OP_POP);
  stmt.body->accept(*this);
  emitLoop(loopStart);
  fixJump(jump);
  currentChunk_->write(OP_POP);
}

void Compiler::visit(ForStmt &stmt) {
  // the initializer's scope.
  beginScope();
  if (stmt.initializer) {
    stmt.initializer->accept(*this);
  }
  int loopStart = currentChunk_->code().size();
  int exitJump = - 1;
  // this solution is more efficient than
  // just push a true value to the stack when
  // there is no condition.
  if (stmt.condition) {
    stmt.condition->accept(*this);
    currentChunk_->write(OP_JUMP_IF_FALSE);
    exitJump = currentChunk_->code().size();
    currentChunk_->write(static_cast<OpCode>(0xff));
    currentChunk_->write(OP_POP);
  }
  stmt.body->accept(*this);
  if (stmt.increment) {
    stmt.increment->accept(*this);
    currentChunk_->write(OP_POP);
  }
  emitLoop(loopStart);
  // pop the condition.
  if (exitJump != -1) {
    fixJump(exitJump);
    currentChunk_->write(OP_POP);
  }
  endScope();
}

void Compiler::visit(PrintStmt &stmt) {
  stmt.expr->accept(*this);
  currentChunk_->write(OP_PRINT);
}

void Compiler::visit(ReturnStmt &stmt) {
  if (isInitializer) {
    compileTimeError("can't use `return` keyword in a initializer.");
    hadError_ = true;
  }
  if (stmt.expr) {
    stmt.expr->accept(*this);
  } else {
    currentChunk_->write(OP_NIL);
  }
  currentChunk_->write(OP_RETURN);
}

void Compiler::visit(ExprStmt &stmt) {
  stmt.expr->accept(*this);
  currentChunk_->write(OP_POP);
}

void Compiler::visit(Assign &expr) {
  int index = resolveLocal(expr.name.lexeme_);
  expr.value->accept(*this);
  if (index != -1) {
    currentChunk_->write(OP_SET_LOCAL);
    currentChunk_->write(static_cast<OpCode>(index));
  } else {
    std::string name(expr.name.lexeme_.data(), expr.name.lexeme_.size());
    int slot = currentChunk_->addConstant(name);
    currentChunk_->write(OP_SET_GLOBAL);
    currentChunk_->write(static_cast<OpCode>(slot));
  }
}

void Compiler::visit(Binary &expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  switch (expr.op.type_) {
    case TOKEN_PLUS: {
      currentChunk_->write(OP_ADD);
      break;
    }
    case TOKEN_MINUS: {
      currentChunk_->write(OP_SUBTRACT);
      break;
    }
    case TOKEN_STAR: {
      currentChunk_->write(OP_MULTIPLY);
      break;
    }
    case TOKEN_SLASH: {
      currentChunk_->write(OP_DIVIDE);
      break;
    }
    case TOKEN_EQUAL_EQUAL: {
      currentChunk_->write(OP_EQUAL);
      break;
    }
    case TOKEN_BANG_EQUAL: {
      currentChunk_->write(OP_EQUAL);
      currentChunk_->write(OP_NOT);
      break;
    }
    case TOKEN_GREATER: {
      currentChunk_->write(OP_GREATER);
      break;
    }
    case TOKEN_GREATER_EQUAL: {
      currentChunk_->write(OP_LESS);
      currentChunk_->write(OP_NOT);
      break;
    }
    case TOKEN_LESS: {
      currentChunk_->write(OP_LESS);
      break;
    }
    case TOKEN_LESS_EQUAL: {
      currentChunk_->write(OP_GREATER);
      currentChunk_->write(OP_NOT);
      break;
    }
    default:
      assert(false);
  }
}

void Compiler::visit(Call &expr) {
  expr.callee->accept(*this);
  for (const auto& arg : expr.arguments) {
    arg->accept(*this);
  }
  currentChunk_->write(OP_CALL);
  currentChunk_->write(static_cast<OpCode>(expr.arguments.size()));
}

void Compiler::visit(Get &expr) {
  expr.object->accept(*this);
  std::string name(expr.name.lexeme_.data(), expr.name.lexeme_.size());
  int index = currentChunk_->addConstant(name);
  currentChunk_->write(OP_GET_PROPERTY);
  currentChunk_->write(static_cast<OpCode>(index));
}

void Compiler::visit(Grouping &expr) {
  expr.expr->accept(*this);
}

void Compiler::visit(Set &expr) {
  expr.object->accept(*this);
  expr.value->accept(*this);
  std::string name(expr.name.lexeme_.data(), expr.name.lexeme_.size());
  int index = currentChunk_->addConstant(name);
  currentChunk_->write(OP_SET_PROPERTY);
  currentChunk_->write(static_cast<OpCode>(index));
}

void Compiler::visit(Unary &expr) {
  expr.right->accept(*this);
  currentChunk_->write(
      expr.op.type_ == TOKEN_BANG ? OP_NOT : OP_NEGATE);
}

void Compiler::visit(Variable &expr) {
  int index = resolveLocal(expr.name.lexeme_);
  if (index != -1) {
    currentChunk_->write(OP_GET_LOCAL);
    currentChunk_->write(static_cast<OpCode>(index));
  } else {
    std::string name(expr.name.lexeme_.data(), expr.name.lexeme_.size());
    int slot = currentChunk_->addConstant(name);
    currentChunk_->write(OP_GET_GLOBAL);
    currentChunk_->write(static_cast<OpCode>(slot));
  }
}

// a && b => a ? b : a.
// a || b => a ? a : b.
void Compiler::visit(Logical &expr) {
  expr.left->accept(*this);
  switch (expr.op.type_) {
    case TOKEN_AND: {
      currentChunk_->write(OP_JUMP_IF_FALSE);
      break;
    }
    case TOKEN_OR: {
      currentChunk_->write(OP_JUMP_IF_TRUE);
      break;
    }
    default:
      assert(false);
  }
  int jump = currentChunk_->code().size();
  // sentinel, the relative offset.
  currentChunk_->write(static_cast<OpCode>(0xff));
  // leave only one value of this expression.
  currentChunk_->write(OP_POP);
  expr.right->accept(*this);
  fixJump(jump);
}

void Compiler::visit(Number &expr) {
  currentChunk_->write(OP_CONSTANT);
  auto index = currentChunk_->addConstant(Value(expr.value));
  currentChunk_->write(static_cast<OpCode>(index));
}

void Compiler::visit(String &expr) {
  currentChunk_->write(OP_CONSTANT);
  Value value(std::in_place_type<std::string>,
              expr.str.data(), expr.str.size());
  auto index = currentChunk_->addConstant(value);
  currentChunk_->write(static_cast<OpCode>(index));
}

void Compiler::visit(Literal &expr) {
  switch (expr.literal) {
    case TOKEN_NIL: {
      currentChunk_->write(OP_NIL);
      break;
    }
    case TOKEN_FALSE: {
      currentChunk_->write(OP_FALSE);
      break;
    }
    case TOKEN_TRUE: {
      currentChunk_->write(OP_TRUE);
      break;
    }
    default:
      assert(false);
  }
}

void Compiler::visit(This &expr) {
  currentChunk_->write(OP_GET_LOCAL);
  currentChunk_->write(static_cast<OpCode>(0));
}

}