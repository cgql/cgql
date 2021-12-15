#include "../../cgqlPch.h"

#include "tokenizer.h"
#include "../../logger/logger.h"

namespace cgql {
namespace internal {

// InvalidTokenType
InvalidTokenType::InvalidTokenType(
  TokenType expected,
  TokenType passed
): msg(
    string("Expected token type ") +
    enumToStr(expected) +
    string(" but got ") +
    enumToStr(passed)
  ) {}

// Token
Token::Token(const TokenType& type)
  : type(type) {}
Token::Token(const TokenType& type, const string& value)
  : type(type), value(value) {}
Token::~Token() {}

// Tokenizer
Tokenizer::Tokenizer(const char* source)
  : source(source),
    cursor(0),
    current(generateToken(TokenType::START_OF_QUERY)) {}
Tokenizer::~Tokenizer() {}
Token Tokenizer::advance() {
  this->current = this->nextToken();
  return this->current;
}

Token generateToken(TokenType type) {
  Token generatedToken(type);
  return generatedToken;
}
Token generateToken(TokenType type, const string& value) {
  Token generatedToken(type, value);
  return generatedToken;
}

Token Tokenizer::tokenizeName() {
  uint16_t* i = &this->cursor;
  string value; 
  for(; *i < this->source.length();) {
    if(
      isNameContinue(this->source[*i])
    ) {
      value.push_back(this->source[*i]);
      this->advanceCursor(1);
      continue;
    }
    break;
  }
  return generateToken(
    TokenType::NAME,
    value
  );
}

Token Tokenizer::nextToken() {
  uint16_t len = this->source.length();

  uint16_t* i = &this->cursor;

  for(; *i < len; *i = *i + 1) {
    if(isNameStart(this->source[*i])) {
      return this->tokenizeName();
    }
    switch ((uint32_t)this->source[*i]) {
      case 0xFEFF:
      case 0x0009:
      case 0x0020:
      case 0x002C:
        continue;
      case 0x007B:
        this->advanceCursor(1);
        return generateToken(TokenType::CURLY_BRACES_L);
      case 0x007D:
        this->advanceCursor(1);
        return generateToken(TokenType::CURLY_BRACES_R);
      case 0x003A:
        this->advanceCursor(1);
        return generateToken(TokenType::COLON);
    }
  }
  return generateToken(TokenType::END_OF_QUERY);
}

void Tokenizer::advanceCursor(int8_t amount) {
  if(this->cursor < this->source.length()) {
    this->cursor += amount;
  }
}

} // internal
} // cgql
