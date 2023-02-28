//
// Created by Alan Huang on 2/23/21.
//

#ifndef ALIEN_AST_H
#define ALIEN_AST_H

#include <token.h>
#include <typedef.h>

#include <iostream>
#include <memory>
#include <vector>

namespace alien {

class ClassDecl;
class FuncDecl;
class VarDecl;
class ConstDecl;
class BlockStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class PrintStmt;
class ReturnStmt;
class ExprStmt;

class Assign;
class Binary;
class Call;
class Get;
class Grouping;
class Set;
class Unary;
class Variable;
class Logical;
class Number;
class String;
class Literal;
class This;

class ExprVisitor;
class StmtVisitor;

class Expr {
public:
  enum ExprType {
    VARIABLE,
    GET,
    OTHER
  };
  virtual void accept(ExprVisitor &visitor) = 0;

  virtual void trace(std::ostream &os) const = 0;

  virtual ExprType getType() const { return OTHER; }

  virtual ~Expr() = default;
};

class ExprVisitor {
public:
  virtual void visit(Assign& expr) = 0;
  virtual void visit(Binary& expr) = 0;
  virtual void visit(Call& expr) = 0;
  virtual void visit(Get& expr) = 0;
  virtual void visit(Grouping& expr) = 0;
  virtual void visit(Set& expr) = 0;
  virtual void visit(Unary& expr) = 0;
  virtual void visit(Variable& expr) = 0;
  virtual void visit(Logical& expr) = 0;
  virtual void visit(Number& expr) = 0;
  virtual void visit(String& expr) = 0;
  virtual void visit(Literal& expr) = 0;
  virtual void visit(This& expr) = 0;
  virtual ~ExprVisitor() = default;
};

class Assign : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  Token name;
  ExprPtr value;
};

class Binary : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

class Call : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  ExprPtr callee;
  std::vector<ExprPtr> arguments;
};

class Get : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;
  ExprType getType() const override { return GET; }

  ExprPtr object;
  Token name;
};

class Grouping : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  ExprPtr expr;
};

class Set : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  ExprPtr object;
  Token name;
  ExprPtr value;
};

class Unary : public Expr {
public:
   void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  Token op;
  ExprPtr right;
};

class Variable : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;
  ExprType getType() const override { return VARIABLE; }

  Token name;
};

// because we are using keywords instead of operator now.
// therefore, we can't use Binary to represent logical now.
class Logical : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

class Number : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  double value;
};

class String : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;

  std::string_view str;
};

class Literal : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override ;

  // for convenience.
  TokenType literal;
};

class This : public Expr {
public:
  void accept(ExprVisitor& visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream& os) const override;
};

class Stmt {
public:
  virtual void accept(StmtVisitor &visitor) = 0;

  virtual void trace(std::ostream &os) const = 0;

  virtual ~Stmt() = default;
};

class StmtVisitor {
public:
  virtual void visit(ClassDecl &decl) = 0;
  virtual void visit(FuncDecl &decl) = 0;
  virtual void visit(VarDecl &decl) = 0;
  virtual void visit(ConstDecl &decl) = 0;
  virtual void visit(BlockStmt &stmt) = 0;
  virtual void visit(IfStmt &stmt) = 0;
  virtual void visit(WhileStmt &stmt) = 0;
  virtual void visit(ForStmt &stmt) = 0;
  virtual void visit(PrintStmt &stmt) = 0;
  virtual void visit(ReturnStmt &stmt) = 0;
  virtual void visit(ExprStmt&stmt) = 0;
  virtual ~StmtVisitor() = default;
};

class ClassDecl : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  Token name;
  std::vector<StmtPtr> methods;
};

class FuncDecl : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  Token name;
  std::vector<Token> parameters;
  StmtPtr body;
};

class VarDecl : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  Token name;
  ExprPtr initializer;
};

class ConstDecl : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  Token name;
  ExprPtr initializer;
};

class BlockStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  std::vector<StmtPtr> stmts;
};

class IfStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;
};

class WhileStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  ExprPtr condition;
  StmtPtr body;
};

class ForStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  StmtPtr initializer;
  ExprPtr condition;
  ExprPtr increment;
  StmtPtr body;
};

class PrintStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  ExprPtr expr;
};

class ReturnStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  ExprPtr expr;
};

class ExprStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override {
    visitor.visit(*this);
  }
  void trace(std::ostream &os) const override;

  ExprPtr expr;
};

}

#endif //ALIEN_AST_H
