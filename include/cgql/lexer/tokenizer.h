#pragma once

#include "cgql/utils.h"

#include <iostream>

namespace cgql {

enum class TokenType {
  NAME,

  INT,
  STRING,
  BLOCK_STRING,

  CURLY_BRACES_L,
  CURLY_BRACES_R,
  BRACES_L,
  BRACES_R,
  SQUARE_BRACES_L,
  SQUARE_BRACES_R,

  START_OF_QUERY,
  END_OF_QUERY,

  COLON,
  BANG,
  AMPERSAND,

  SPREAD,
  EQUAL,
  PIPE,
  HASH,
  DOUBLE_QUOTE,
  AT
};

const char* tokenTypeToCharArray(const TokenType& type);
inline std::ostream& operator<<(std::ostream& os, TokenType type) {
  os << tokenTypeToCharArray(type);
  return os;
}

struct Token {
  std::string value;
  TokenType type;
};

class Tokenizer {
public:
  Tokenizer(const char* source);
  ~Tokenizer();
  Token nextToken();
  Token current;
  void advance();
  void skipComments();

  std::string lookAhead();
private:
  std::string source;
  size_t cursor;
  void advanceCursor(int amount);

  Token tokenizeName();
  Token tokenizeDigits();
  Token tokenizeString();
  Token tokenizeBlockString();
};

} // cgql
