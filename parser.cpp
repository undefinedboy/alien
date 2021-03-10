//
// Created by Alan Huang on 2/23/21.
//

#include "token.h"
#include "parser.h"
#include "typedef.h"

#include <vector>
#include <utility>

namespace alien {

Parser::Parser(std::string_view source)
: lexer_(source) {}

std::vector<StmtPtr> Parser::parse() {
  advance();
  std::vector<StmtPtr> decls;
  while (!match(TOKEN_EOF)) {
    decls.push_back(parseDecl());
  }
  return decls;
}

StmtPtr Parser::parseDecl() {
  StmtPtr decl;
  if (match(TOKEN_CLASS)) {
    decl = parseClassDecl();
  } else if (match(TOKEN_FUNC)) {
    decl = parseFuncDecl(TYPE_FUNCTION);
  } else if (match(TOKEN_VAR)){
    decl = parseVarDecl();
  }
  if (panicMode_) {
    synchronize();
  }
  return decl;
}

StmtPtr Parser::parseClassDecl() {
  auto classDecl = std::make_unique<ClassDecl>();
  consume(TOKEN_IDENTIFIER, "Expect class name.");
  classDecl->name = previous_;
  consume(TOKEN_LEFT_BRACE, "Expect '{' after class name.");
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    classDecl->methods.push_back(parseFuncDecl(TYPE_METHOD));
  }
  consume(TOKEN_RIGHT_BRACE, "Expect '}' after class declaration.");
  return classDecl;
}

StmtPtr Parser::parseFuncDecl(FunctionType type) {
  auto funcDecl = std::make_unique<FuncDecl>();
  if (type == TYPE_METHOD) {
    consume(TOKEN_FUNC, "Expect 'func' before function declaration.");
  }
  consume(TOKEN_IDENTIFIER, "Expect func name.");
  funcDecl->name = previous_;
  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      consume(TOKEN_IDENTIFIER, "Expect parameter name.");
      funcDecl->parameters.push_back(previous_);
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
  funcDecl->body = parseBlock();
  return funcDecl;
}

StmtPtr Parser::parseVarDecl() {
  auto varDecl = std::make_unique<VarDecl>();
  consume(TOKEN_IDENTIFIER, "Expect variable name.");
  varDecl->name = previous_;
  if (match(TOKEN_EQUAL)) {
    varDecl->initializer = parseExpr();
  }
  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
  return varDecl;
}


StmtPtr Parser::parseStmt() {
  if (match(TOKEN_IF)) {
    return parseIfStmt();
  } else if (match(TOKEN_WHILE)) {
    return parseWhileStmt();
  } else if (match(TOKEN_FOR)) {
    return parseForStmt();
  } else if (match(TOKEN_PRINT)) {
    return parsePrintStmt();
  } else if (match(TOKEN_RETURN)) {
    return parseReturnStmt();
  } else if (match(TOKEN_LEFT_BRACE)) {
    return parseBlock();
  } else {
    return parseExprStmt();
  }
}

StmtPtr Parser::parseIfStmt() {
  auto ifStmt = std::make_unique<IfStmt>();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
  ifStmt->condition = parseExpr();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' after ')'.");
  ifStmt->thenBranch = parseBlock();
  if (match(TOKEN_ELSE)) {
    consume(TOKEN_LEFT_BRACE, "Expect '{' after 'else'.");
    ifStmt->elseBranch = parseBlock();
  }
  return ifStmt;
}

StmtPtr Parser::parseWhileStmt() {
  auto whileStmt = std::make_unique<WhileStmt>();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  whileStmt->condition = parseExpr();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' after ')'.");
  whileStmt->body = parseBlock();
  return whileStmt;
}

StmtPtr Parser::parseForStmt() {
  auto forStmt = std::make_unique<ForStmt>();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  if (match(TOKEN_SEMICOLON)) {
    // there is no initializer;
  } else if (match(TOKEN_VAR)) {
    forStmt->initializer = parseVarDecl();
  } else {
    forStmt->initializer = parseExprStmt();
  }

  if (!match(TOKEN_SEMICOLON)) {
    forStmt->condition = parseExpr();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");
  }

  if (!match(TOKEN_RIGHT_PAREN)) {
    forStmt->increment = parseExpr();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");
  }
  consume(TOKEN_LEFT_BRACE, "Expect '{' after ')'.");
  forStmt->body = parseBlock();
  return forStmt;
}

StmtPtr Parser::parseExprStmt() {
  auto exprStmt = std::make_unique<ExprStmt>();
  exprStmt->expr = parseExpr();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  return exprStmt;
}

StmtPtr Parser::parseReturnStmt() {
  auto returnStmt = std::make_unique<ReturnStmt>();
  if (!check(TOKEN_SEMICOLON)) {
    returnStmt->expr = parseExpr();
  }
  consume(TOKEN_SEMICOLON,  "Expect ';' after return statement.");
  return returnStmt;
}

StmtPtr Parser::parsePrintStmt() {
  auto printStmt = std::make_unique<PrintStmt>();
  printStmt->expr = parseExpr();
  consume(TOKEN_SEMICOLON, "Expect ';' after print statement.");
  return printStmt;
}

StmtPtr Parser::parseBlock() {
  auto block = std::make_unique<BlockStmt>();
  std::vector<StmtPtr> statements;
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    if (match(TOKEN_VAR)) {
      block->stmts.push_back(parseVarDecl());
    } else {
      block->stmts.push_back(parseStmt());
    }
  }
  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
  return block;
}

ExprPtr Parser::parseExpr() {
  return parseAssign();
}

ExprPtr Parser::parseAssign() {
  auto expr = parseOr();
  if (match(TOKEN_EQUAL)) {
    // decrease one layer of indirection
    auto value = parseAssign();
    if (expr->getType() == Expr::VARIABLE) {
      auto assignExpr = std::make_unique<Assign>();
      auto v = dynamic_cast<Variable*>(expr.get());
      assignExpr->name = v->name;
      assignExpr->value = std::move(value);
      return assignExpr;
    } else if (expr->getType() == Expr::GET) {
      auto setExpr = std::make_unique<Set>();
      auto get = dynamic_cast<Get*>(expr.get());
      setExpr->object = std::move(get->object);
      setExpr->name = get->name;
      setExpr->value = std::move(value);
      return setExpr;
    }
    error("Invalid assignment target.");
  }
  return expr;
}

ExprPtr Parser::parseOr() {
  auto expr = parseAnd();
  while (match(TOKEN_OR)) {
    auto orExpr = std::make_unique<Logical>();
    orExpr->op = previous_;
    orExpr->left = std::move(expr);
    orExpr->right = parseAnd();
    expr = std::move(orExpr);
  }
  return expr;
}

ExprPtr Parser::parseAnd() {
  auto expr = parseEquality();
  while (match(TOKEN_AND)) {
    auto andExpr = std::make_unique<Logical>();
    andExpr->op = previous_;
    andExpr->left = std::move(expr);
    andExpr->right = parseEquality();
    expr = std::move(andExpr);
  }
  return expr;
}

ExprPtr Parser::parseEquality() {
  auto expr = parseCmp();
  while (match(TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
    auto equalityExpr = std::make_unique<Binary>();
    equalityExpr->op = previous_;
    equalityExpr->left = std::move(expr);
    equalityExpr->right = parseCmp();
    expr = std::move(equalityExpr);
  }
  return expr;
}

ExprPtr Parser::parseCmp() {
  auto expr = parseTerm();
  while (match(TOKEN_GREATER, TOKEN_GREATER_EQUAL,
      TOKEN_LESS, TOKEN_LESS_EQUAL)) {
    auto cmpExpr = std::make_unique<Binary>();
    cmpExpr->op = previous_;
    cmpExpr->left = std::move(expr);
    cmpExpr->right = parseTerm();
    expr = std::move(cmpExpr);
  }
  return expr;
}

ExprPtr Parser::parseTerm() {
  auto expr = parseFactor();
  while (match(TOKEN_PLUS, TOKEN_MINUS)) {
    auto termExpr = std::make_unique<Binary>();
    termExpr->op = previous_;
    termExpr->left = std::move(expr);
    termExpr->right = parseFactor();
    expr = std::move(termExpr);
  }
  return expr;
}

ExprPtr Parser::parseFactor() {
  auto expr = parseUnary();
  while (match(TOKEN_SLASH, TOKEN_STAR)) {
    auto factorExpr = std::make_unique<Binary>();
    factorExpr->op = previous_;
    factorExpr->left = std::move(expr);
    factorExpr->right = parseUnary();
    expr = std::move(factorExpr);
  }
  return expr;
}

ExprPtr Parser::parseUnary() {
  if (match(TOKEN_BANG, TOKEN_MINUS)) {
    auto unaryExpr = std::make_unique<Unary>();
    unaryExpr->op = previous_;
    unaryExpr->right = parseUnary();
    return unaryExpr;
  }
  return parseCall();
}

ExprPtr Parser::parseArgs(ExprPtr callee) {
  auto expr = std::make_unique<Call>();
  expr->callee = std::move(callee);
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expr->arguments.push_back(parseExpr());
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return expr;
}


ExprPtr Parser::parseCall() {
  auto expr = parsePrimary();
  while (true) {
    if (match(TOKEN_LEFT_PAREN)) {
      expr = parseArgs(std::move(expr));
    } else if (match(TOKEN_DOT)) {
      consume(TOKEN_IDENTIFIER, "Expect identifier after '.'.");
      auto getExpr = std::make_unique<Get>();
      getExpr->name = previous_;
      getExpr->object = std::move(expr);
      expr = std::move(getExpr);
    } else {
      break;
    }
  }
  return expr;
}

ExprPtr Parser::parsePrimary() {
  if (match(TOKEN_NIL, TOKEN_FALSE, TOKEN_TRUE)) {
    auto literalExpr = std::make_unique<Literal>();
    literalExpr->literal = previous_.type_;
    return literalExpr;
  }
  if (match(TOKEN_THIS)) {
    return std::make_unique<This>();
  }
  if (match(TOKEN_NUMBER)) {
    auto numberExpr = std::make_unique<Number>();
    // attention: 'strtod' may interpret 123.123e12 to valid number.
    numberExpr->value = std::strtod(previous_.lexeme_.data(), nullptr);
    return numberExpr;
  }
  if (match(TOKEN_STRING)) {
    auto stringExpr = std::make_unique<String>();
    stringExpr->str = previous_.lexeme_;
    return stringExpr;
  }
  if (match(TOKEN_IDENTIFIER)) {
    auto variable = std::make_unique<Variable>();
    variable->name = previous_;
    return variable;
  }
  if (match(TOKEN_LEFT_PAREN)) {
    auto grouping = std::make_unique<Grouping>();
    grouping->expr = parseExpr();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after grouping expression");
    return grouping;
  }
  error("Expect expression");
  return nullptr;
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

// we can put the template here because we are using it only in this class.
template <typename T, typename... Args>
bool Parser::match(T t, Args... args) {
  return match(t) || match(args...);
}

bool Parser::check(TokenType type) {
  return current_.type_ == type;
}

void Parser::consume(TokenType expected, std::string_view message) {
  if (current_.type_ == expected) {
    advance();
    return;
  }
  errorAtCurrent(message);
}

void Parser::advance() {
  previous_ = current_;
  for (;;) {
    current_ = lexer_.nextToken();
    if (current_.type_ != TOKEN_ERROR) return;
    errorAtCurrent(current_.lexeme_);
  }
}

bool Parser::hadError() const {
  return hadError_;
}

void Parser::error(std::string_view message) {
  errorAt(message);
}

void Parser::errorAtCurrent(std::string_view message) {
  errorAt(message, true);
}

void Parser::errorAt(std::string_view message, bool isCurrent) {
  // avoid endless token error.
  if (panicMode_) return;
  panicMode_ = true;
  hadError_ = true;
  Token errorToken = isCurrent ? current_ : previous_;
  std::cerr << "[line " << errorToken.line_ << "] Error";
  if (errorToken.type_ == TOKEN_EOF) {
    std::cerr << " at end";
  } else {
    std::cerr << " at " << "'" << errorToken.lexeme_ << "'";
  }
  std::cerr << ": " << message << "\n";
}

// synchronize syntax errors.
void Parser::synchronize() {
  // report subsequent errors, if any.
  panicMode_ = false;
  while (current_.type_ != TOKEN_EOF) {
    // whether the previous token can end a statement.
    if (previous_.type_ == TOKEN_SEMICOLON) return;
    switch (current_.type_) {
      case TOKEN_CLASS:
      case TOKEN_FUNC:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;
      default:
        ;
    }
    advance();
  }
}


}
