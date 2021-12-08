#include "../../cgqlPch.h"

#include "utils.h"

#define enumToStr(enumValue) #enumValue

using std::string;
using std::exception;

enum TokenType {
  DOCUMENT = 0,
  OPERATION_DEFINITION = 1,
  SELECTION_SET = 2,
  FIELD = 3,
  NAME = 4,

  INT = 5,
  STRING = 6,

  CURLY_BRACES_L = 7,
  CURLY_BRACES_R = 8,

  START_OF_QUERY = 9,
  END_OF_QUERY = 10
};

class InvalidTokenType : public exception {
public:
  InvalidTokenType(TokenType expected, TokenType passed);
  virtual const char* what() const throw() {
    return this->msg.c_str();
  }
private:
  string msg;
};

class UnexpectedEnd : public exception {
public:
  virtual const char* what() const throw() {
    return "Unexpected end of input";
  }
};

class Token {
public:
  Token(TokenType type);
  Token(TokenType type, string value);
  ~Token();

  inline TokenType getType() const { return this->type; }
  inline string getValue() const { return this->value; }
private:
  TokenType type;
  string value;
};

class Tokenizer {
public:
  Tokenizer(string source);
  ~Tokenizer();
  Token nextToken();
  Token current;
  Token advance();
private:
  string source;
  uint16_t cursor;
  void advanceCursor(int8_t amount);

  Token tokenizeName();
};

Token generateToken(TokenType type, string value);
Token generateToken(TokenType type);
