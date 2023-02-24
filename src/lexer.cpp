//
// Created by Alan Huang on 2021-02-20.
//

#include <lexer.h>
#include <token.h>

#include <string_view>
#include <unordered_map>

#include <cassert>

namespace alien {

std::unordered_map<std::string_view, TokenType> Lexer::keywords_{
    { "if",     TOKEN_IF },
    { "else",   TOKEN_ELSE },
    { "while",  TOKEN_WHILE },
    { "for",    TOKEN_FOR },
    { "func",   TOKEN_FUNC },
    { "class",  TOKEN_CLASS },
    { "var",    TOKEN_VAR},
    { "true",   TOKEN_TRUE },
    { "false",  TOKEN_FALSE },
    { "nil",    TOKEN_NIL },
    { "print",  TOKEN_PRINT },
    { "return", TOKEN_RETURN },
    { "this",   TOKEN_THIS },
    { "or",     TOKEN_OR },
    { "and",    TOKEN_AND }
};

Token Lexer::nextToken() {
  skipWhitespace();
  tokenStart_ = pos_;
  if (isAtEnd()) {
    return Token(TOKEN_EOF, line_);
  }
  char c = advance();
  if (isName(c)) {
    return identifier();
  }
  if (isDigit(c)) {
    return number();
  }
  switch (c) {
    case '+': return Token(TOKEN_PLUS, line_);
    case '-': return Token(TOKEN_MINUS, line_);
    case '*': return Token(TOKEN_STAR, line_);
    case '/': return Token(TOKEN_SLASH, line_);
    case '(': return Token(TOKEN_LEFT_PAREN, line_);
    case ')': return Token(TOKEN_RIGHT_PAREN, line_);
    case '{': return Token(TOKEN_LEFT_BRACE, line_);
    case '}': return Token(TOKEN_RIGHT_BRACE, line_);
    case ',': return Token(TOKEN_COMMA, line_);
    case ';': return Token(TOKEN_SEMICOLON, line_);
    case '.': return Token(TOKEN_DOT, line_);
    case '>':
      return Token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER, line_);
    case '<':
      return Token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS, line_);
    case '!':
      return Token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG, line_);
    case '=':
      return Token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL, line_);
    case '"': return string();
    default:
      return Token(TOKEN_ERROR, "Unknown character.", line_);
  }
}

bool Lexer::isDigit(char c) const {
  return c >= '0' && c <= '9';
}

bool Lexer::isName(char c) const {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAtEnd() const {
  return pos_ >= source_.size();
}

char Lexer::peek(int ahead) const {
  if (pos_ + ahead >= source_.size()) return '\0';
  return source_[pos_ + ahead];
}

bool Lexer::match(char expected) {
  if (peek() != expected) return false;
  advance();
  return true;
}

char Lexer::advance() {
  assert(pos_ < source_.size());
  return source_[pos_++];
}

void Lexer::skipWhitespace() {
  for (;;) {
    // we have to use peek in case of that c is not a whitespace.
    char c = peek();
    switch (c) {
      case ' ':
      case'\r':
      case'\t':
        advance();
        break;
      case '\n':
        line_++;
        advance();
        break;
      default:
        return;
    }
  }
}

Token Lexer::identifier() {
  while (isName(peek()) || isDigit(peek())) {
    advance();
  }
  auto lexeme = source_.substr(tokenStart_, pos_ - tokenStart_);
  if (keywords_.find(lexeme) != keywords_.end()) {
    return Token(keywords_[lexeme], lexeme, line_);
  }
  return Token(TOKEN_IDENTIFIER, lexeme, line_);
}

Token Lexer::number() {
  while (isDigit(peek())) {
    advance();
  }
  if (peek() == '.' && isDigit(peek(1))) {
    advance();
    while (isDigit(peek())) {
      advance();
    }
  }
  auto lexeme = source_.substr(tokenStart_, pos_ - tokenStart_);
  return Token(TOKEN_NUMBER, lexeme, line_);
}

Token Lexer::string() {
  // '"' appears first, in most cases.
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line_++;
    advance();
  }
  if (isAtEnd()) {
    return Token(TOKEN_ERROR, "Unterminated string.", line_);
  }
  advance();
  // delete the two '"'.
  auto lexeme = source_.substr(tokenStart_ + 1, pos_ - tokenStart_ - 2);
  return Token(TOKEN_STRING, lexeme, line_);
}

}