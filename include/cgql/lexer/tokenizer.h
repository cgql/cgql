#pragma once

#include <string>

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

template<typename Ostream>
Ostream& operator<<(Ostream& os, TokenType type) {
  switch (type) {
    case TokenType::NAME:
      os << "NAME";
      break;
    case TokenType::INT:
      os << "INT";
      break;
    case TokenType::STRING:
      os << "STRING";
      break;
    case TokenType::BLOCK_STRING:
      os << "BLOCK_STRING";
      break;
    case TokenType::CURLY_BRACES_L:
      os << "CURLY_BRACES_L";
      break;
    case TokenType::CURLY_BRACES_R:
      os << "CURLY_BRACES_R";
      break;
    case TokenType::BRACES_L:
      os << "BRACES_L";
      break;
    case TokenType::BRACES_R:
      os << "BRACES_R";
      break;
    case TokenType::SQUARE_BRACES_L:
      os << "SQUARE_BRACES_L";
      break;
    case TokenType::SQUARE_BRACES_R:
      os << "SQUARE_BRACES_R";
      break;
    case TokenType::START_OF_QUERY:
      os << "START_OF_QUERY";
      break;
    case TokenType::END_OF_QUERY:
      os << "END_OF_QUERY";
      break;
    case TokenType::COLON:
      os << "COLON";
      break;
    case TokenType::BANG:
      os << "BANG";
      break;
    case TokenType::AMPERSAND:
      os << "AMPERSAND";
      break;
    case TokenType::SPREAD:
      os << "SPREAD";
      break;
    case TokenType::EQUAL:
      os << "EQUAL";
      break;
    case TokenType::PIPE:
      os << "PIPE";
      break;
    case TokenType::HASH:
      os << "HASH";
      break;
    case TokenType::DOUBLE_QUOTE:
      os << "DOUBLE_QUOTE";
      break;
    case TokenType::AT:
      os << "AT";
      break;
  }
  return os;
}

struct Token {
  std::string value;
  TokenType type;
};

class Tokenizer {
public:
  Tokenizer(const char* source);
  Token current;
  void advance();

  std::string lookAhead();
private:
  std::string source;
  size_t cursor = 0;

  void advanceCursor(int amount);
  void skipComments();
  Token nextToken();

  Token tokenizeName();
  Token tokenizeDigits();
  Token tokenizeString();
  Token tokenizeBlockString();
};

} // cgql
