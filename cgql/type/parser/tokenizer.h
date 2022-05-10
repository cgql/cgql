#pragma once

#include "cgql/base/cgqlPch.h"

#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

enum TokenType {
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
  DOUBLE_QUOTE
};

const char* tokenTypeToCharArray(const TokenType& type);
inline std::ostream& operator<<(std::ostream& os, TokenType type) {
  os << tokenTypeToCharArray(type);
  return os;
}

class Token {
public:
  Token(TokenType type);
  Token(TokenType type, const std::string& value);
  ~Token();

  inline TokenType getType() const { return this->type; }
  inline const std::string& getValue() const { return this->value; }
private:
  TokenType type;
  std::string value;
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
  void advanceCursor(int8_t amount);

  Token tokenizeName();
  Token tokenizeDigits();
  Token tokenizeString();
  Token tokenizeBlockString();
};

Token generateToken(TokenType type, const std::string& value);
Token generateToken(TokenType type);

} // internal
} // cgql
