//
// Created by Alan Huang on 2021-02-22.
//

#ifndef ALIEN_TOKEN_H
#define ALIEN_TOKEN_H

#include <string_view>


namespace alien {

enum TokenType {
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR,
  TOKEN_SLASH, TOKEN_COMMA, TOKEN_SEMICOLON,
  TOKEN_DOT,

  // about compare
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,

  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

  // keywords
  TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR,
  TOKEN_FUNC, TOKEN_CLASS, TOKEN_VAR, TOKEN_TRUE,
  TOKEN_FALSE, TOKEN_NIL, TOKEN_PRINT, TOKEN_RETURN,
  TOKEN_THIS, TOKEN_OR, TOKEN_AND,

  TOKEN_ERROR,
  TOKEN_EOF
};

struct Token {
  Token();
  Token(const TokenType type, const int line);
  Token(const TokenType type, std::string_view lexeme, const int line);
  TokenType type_;
  std::string_view lexeme_;
  int line_;
};

}

#endif //ALIEN_TOKEN_H
