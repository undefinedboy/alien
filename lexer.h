//
// Created by Alan Huang on 2021-02-20.
//

#ifndef ALIEN_LEXER_H
#define ALIEN_LEXER_H

#include "token.h"

#include <string_view>
#include <unordered_map>

namespace alien {

class Lexer {
public:
  explicit Lexer(std::string_view source)
  : source_(source) {}
  Token nextToken();

private:
  Token identifier();
  Token number();
  Token string();

private:
  bool isDigit(char c) const;
  bool isName(char c) const;
  bool isAtEnd() const;

  char peek(int ahead = 0) const;
  bool match(char expected);
  char advance();
  void skipWhitespace();
private:
  std::string_view source_;
  static std::unordered_map<std::string_view, TokenType> keywords_;

  int pos_ = 0;
  int tokenStart_ = 0;
  int line_ = 1;
};

}


#endif //ALIEN_LEXER_H
