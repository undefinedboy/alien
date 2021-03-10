//
// Created by Alan Huang on 2/23/21.
//

#ifndef ALIEN_PARSER_H
#define ALIEN_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "typedef.h"

#include <memory>
#include <vector>
#include <string_view>

namespace alien {

class Parser {
public:
  explicit Parser(std::string_view);
  std::vector<StmtPtr> parse();
  bool hadError() const;
private:
  enum FunctionType {
    TYPE_METHOD,
    TYPE_FUNCTION
  };
private:
  StmtPtr parseDecl();
  StmtPtr parseClassDecl();
  StmtPtr parseFuncDecl(FunctionType type);
  StmtPtr parseVarDecl();
  StmtPtr parseStmt();
  StmtPtr parseIfStmt();
  StmtPtr parseWhileStmt();
  StmtPtr parseForStmt();
  StmtPtr parseExprStmt();
  StmtPtr parseReturnStmt();
  StmtPtr parsePrintStmt();
  StmtPtr parseBlock();
  ExprPtr parseExpr();
  ExprPtr parseAssign();
  ExprPtr parseOr();
  ExprPtr parseAnd();
  ExprPtr parseEquality();
  ExprPtr parseCmp();
  ExprPtr parseTerm();
  ExprPtr parseFactor();
  ExprPtr parseUnary();
  ExprPtr parseCall();
  ExprPtr parsePrimary();
  ExprPtr parseArgs(ExprPtr callee);
private:
  bool match(TokenType type);
  template <typename T, typename... Args>
  bool match(T t, Args... args);
  bool check(TokenType type);
  void consume(TokenType expected, std::string_view message);
  void advance();
private:
  void error(std::string_view message);
  void errorAtCurrent(std::string_view message);
  void errorAt(std::string_view message, bool isCurrent = false);
  void synchronize();
private:
  bool panicMode_ = false;
  bool hadError_  = false;
  Token previous_;
  Token current_;
  Lexer lexer_;
};


}

#endif //ALIEN_PARSER_H
