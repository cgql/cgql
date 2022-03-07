#include "cgql/type/parser/tokenizer.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

const char* tokenTypeToCharArray(const TokenType& type) {
  switch (type) {
    case TokenType::NAME:
      return "NAME";
      break;
    case TokenType::INT:
      return "INT";
      break;
    case TokenType::STRING:
      return "STRING";
      break;
    case TokenType::BLOCK_STRING:
      return "BLOCK_STRING";
      break;
    case TokenType::CURLY_BRACES_L:
      return "CURLY_BRACES_L";
      break;
    case TokenType::CURLY_BRACES_R:
      return "CURLY_BRACES_R";
      break;
    case TokenType::BRACES_L:
      return "BRACES_L";
      break;
    case TokenType::BRACES_R:
      return "BRACES_R";
      break;
    case TokenType::SQUARE_BRACES_L:
      return "SQUARE_BRACES_L";
      break;
    case TokenType::SQUARE_BRACES_R:
      return "SQUARE_BRACES_R";
      break;
    case TokenType::START_OF_QUERY:
      return "START_OF_QUERY";
      break;
    case TokenType::END_OF_QUERY:
      return "END_OF_QUERY";
      break;
    case TokenType::COLON:
      return "COLON";
      break;
    case TokenType::BANG:
      return "BANG";
      break;
    case TokenType::AMPERSAND:
      return "AMPERSAND";
      break;
    case TokenType::SPREAD:
      return "SPREAD";
      break;
    case TokenType::EQUAL:
      return "EQUAL";
      break;
    case TokenType::PIPE:
      return "PIPE";
      break;
    case TokenType::HASH:
      return "HASH";
      break;
    case TokenType::DOUBLE_QUOTE:
      return "DOUBLE_QUOTE";
      break;
  }
  return "";
}

// Token
Token::Token(TokenType type)
  : type(type) {}
Token::Token(TokenType type, const std::string& value)
  : type(type), value(value) {}

Token::~Token() {}

// Tokenizer
Tokenizer::Tokenizer(const char* source)
  : source(source),
    cursor(0),
    current(generateToken(TokenType::START_OF_QUERY)) {}
Tokenizer::~Tokenizer() {}
void Tokenizer::advance() {
  this->current = this->nextToken();
}

std::string Tokenizer::lookAhead() {
  if(
    this->current.getType() != TokenType::STRING &&
    this->current.getType() != TokenType::BLOCK_STRING
  ) return this->current.getValue();
  uint16_t oldCursor = this->cursor;
  Token next = this->nextToken();
  this->cursor = oldCursor;
  return next.getValue();
}

Token generateToken(TokenType type) {
  Token generatedToken(type);
  return generatedToken;
}
Token generateToken(TokenType type, const std::string& value) {
  Token generatedToken(type, value);
  return generatedToken;
}

Token Tokenizer::tokenizeName() {
  uint16_t* i = &this->cursor;
  std::string value; 
  for(; *i < this->source.length();) {
    if(
      isNameContinue(this->source[*i])
    ) {
      value += this->source[*i];
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

Token Tokenizer::tokenizeDigits() {
  uint16_t* i = &this->cursor;
  std::string value;
  for(; *i < this->source.length();) {
    if(isDigit(this->source[*i])) {
      value += this->source[*i];
      this->advanceCursor(1);
      continue;
    }
    break;
  }
  return generateToken(
    TokenType::INT,
    value
  );
}

void Tokenizer::skipComments() {
  uint16_t* i = &this->cursor;

  for(; *i < this->source.length();) {
    if(
      this->source[*i] == 0x000A ||
      this->source[*i] == 0x000D
    ) {
      break;
    }
    this->advanceCursor(1);
  }
}

Token Tokenizer::tokenizeString() {
  this->advanceCursor(1);

  std::string tokenizedString;

  uint16_t* i = &this->cursor;
  for(; *i < this->source.length();) {
    if(this->source[*i] == 0x0022) break;
    tokenizedString += this->source[*i];
    this->advanceCursor(1);
  }
  this->advanceCursor(1);
  return generateToken(
    TokenType::STRING,
    tokenizedString
  );
}

Token Tokenizer::tokenizeBlockString() {
  this->advanceCursor(3);

  std::string blockString;

  uint16_t* i = &this->cursor;
  for(; *i < this->source.length();) {
    if(
      this->source[*i]     == 0x0022 &&
      this->source[*i + 1] == 0x0022 &&
      this->source[*i + 2] == 0x0022
    ) break;
    blockString += this->source[*i];
    this->advanceCursor(1);
  }
  this->advanceCursor(3);
  return generateToken(
    TokenType::BLOCK_STRING,
    blockString
  );
}

Token Tokenizer::nextToken() {
  size_t len = this->source.length();

  uint16_t* i = &this->cursor;

  for(; *i < len; *i = *i + 1) {
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
      case 0x005B:
        this->advanceCursor(1);
        return generateToken(TokenType::SQUARE_BRACES_L);
      case 0x005D:
        this->advanceCursor(1);
        return generateToken(TokenType::SQUARE_BRACES_R);
      case 0x0028:
        this->advanceCursor(1);
        return generateToken(TokenType::BRACES_L);
      case 0x0029:
        this->advanceCursor(1);
        return generateToken(TokenType::BRACES_R);
      case 0x003A:
        this->advanceCursor(1);
        return generateToken(TokenType::COLON);
      case 0x0021:
        this->advanceCursor(1);
        return generateToken(TokenType::BANG);
      case 0x0026:
        this->advanceCursor(1);
        return generateToken(TokenType::AMPERSAND);
      case 0x002E:
        if(
          this->source[*i + 1] == 0x002E &&
          this->source[*i + 2] == 0x002E
        ) {
          this->advanceCursor(3);
          return generateToken(TokenType::SPREAD);
        }
      case 0x003D:
        this->advanceCursor(1);
        return generateToken(TokenType::EQUAL);
      case 0x007C:
        this->advanceCursor(1);
        return generateToken(TokenType::PIPE);
      case 0x0023:
        this->advanceCursor(1);
        this->skipComments();
        continue;
      case 0x0022:
        if(
          this->source[*i + 1] == 0x0022 &&
          this->source[*i + 2] == 0x0022
        ) {
          return this->tokenizeBlockString();
        }
        return this->tokenizeString();
    }
    if(isDigit(this->source[*i])) {
      return this->tokenizeDigits();
    }
    if(isNameStart(this->source[*i])) {
      return this->tokenizeName();
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
