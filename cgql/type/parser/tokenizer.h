#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLScalar.h"
#include "cgql/type/parser/utils.h"
#include "cgql/type/location.h"

namespace cgql {
namespace internal {

enum TokenType {
  DOCUMENT,
  OPERATION_DEFINITION,
  SELECTION_SET,
  FIELD,
  NAME,

  INT,
  STRING,

  CURLY_BRACES_L,
  CURLY_BRACES_R,
  BRACES_L,
  BRACES_R,

  START_OF_QUERY,
  END_OF_QUERY,

  COLON
};

const char* tokenTypeToCharArray(const TokenType& type);
inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {
  os << tokenTypeToCharArray(type);
  return os;
}

class Token {
public:
  Token(const TokenType& type);
  Token(const TokenType& type, const std::string& value);
  ~Token();

  constexpr inline TokenType getType() const { return this->type; }
  constexpr inline const std::string& getValue() const { return this->value; }
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
  Token advance();
private:
  std::string source;
  uint16_t cursor;
  void advanceCursor(int8_t amount);

  Token tokenizeName();
  Token tokenizeDigits();
};

Token generateToken(TokenType type, const std::string& value);
Token generateToken(TokenType type);

} // internal
} // cgql

#endif
