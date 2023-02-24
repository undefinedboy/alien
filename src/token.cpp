//
// Created by Alan Huang on 2/22/21.
//

#include <token.h>

#include <string_view>

#include <cassert>

namespace alien {

std::string_view typeString(const TokenType type) {
  switch (type) {
    case TOKEN_PLUS:          return "+";
    case TOKEN_MINUS:         return "-";
    case TOKEN_STAR:          return "*";
    case TOKEN_SLASH:         return "/";
    case TOKEN_LEFT_PAREN:    return "(";
    case TOKEN_RIGHT_PAREN:   return ")";
    case TOKEN_LEFT_BRACE:    return "{";
    case TOKEN_RIGHT_BRACE:   return "}";
    case TOKEN_COMMA:         return ",";
    case TOKEN_SEMICOLON:     return ";";
    case TOKEN_DOT:           return ".";
    case TOKEN_BANG:          return "!";
    case TOKEN_EQUAL:         return "=";
    case TOKEN_EQUAL_EQUAL:   return "==";
    case TOKEN_BANG_EQUAL:    return "!=";
    case TOKEN_LESS:          return "<";
    case TOKEN_LESS_EQUAL:    return "<=";
    case TOKEN_GREATER:       return ">";
    case TOKEN_GREATER_EQUAL: return ">=";
    case TOKEN_EOF:           return "EOF";
    default:
      assert(false);
  }
  return "ERROR";
}

Token::Token()
: type_(TOKEN_ERROR), lexeme_("Uninitialized token."), line_(-1) {}

Token::Token(const TokenType type, const int line)
: type_(type), lexeme_(typeString(type)), line_(line) {}

Token::Token(const TokenType type, std::string_view lexeme, const int line)
: type_(type), lexeme_(lexeme), line_(line) {}

}