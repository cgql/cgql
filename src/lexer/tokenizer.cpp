#include "cgql/lexer/tokenizer.h"
#include "cgql/logger/logger.h"
#include "../utils.h"

namespace cgql {

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
    case TokenType::AT:
      return "AT";
      break;
  }
  return "";
}

static inline Token generateToken(TokenType type) {
  Token generatedToken {
    .type = type 
  };
  return generatedToken;
}
static inline Token generateToken(TokenType type, std::string value) {
  Token generatedToken {
    .value = value,
    .type = type
  };
  return generatedToken;
}

// Tokenizer
Tokenizer::Tokenizer(const char* source)
  : source(source),
    current(generateToken(TokenType::START_OF_QUERY)) {}
void Tokenizer::advance() {
  this->current = this->nextToken();
}

std::string Tokenizer::lookAhead() {
  if(
    this->current.type != TokenType::STRING &&
    this->current.type != TokenType::BLOCK_STRING
  ) return this->current.value;
  size_t oldCursor = this->cursor;
  Token next = this->nextToken();
  this->cursor = oldCursor;
  return next.value;
}

Token Tokenizer::tokenizeName() {
  size_t end = this->cursor;
  while(end < this->source.length()) {
    if(!isNameContinue(this->source[end]))
      break;
    ++end;
  }
  std::string value =
    this->source.substr(this->cursor, end - this->cursor);
  cursor = end;
  return generateToken(
    TokenType::NAME,
    value
  );
}

Token Tokenizer::tokenizeDigits() {
  size_t end = this->cursor;
  while(end < this->source.length()) {
    if(!isDigit(this->source[end]))
      break;
    ++end;
  }
  std::string value =
    this->source.substr(this->cursor, end - this->cursor);
  cursor = end;
  return generateToken(
    TokenType::INT,
    value
  );
}

void Tokenizer::skipComments() {
  size_t* i = &this->cursor;

  while(*i < this->source.length()) {
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

  size_t end = this->cursor;
  while(end < this->source.length()) {
    if(this->source[end] == 0x0022) break;
    ++end;
  }
  std::string value =
    this->source.substr(this->cursor, end - this->cursor);
  cursor = end + 1;
  return generateToken(
    TokenType::STRING,
    value
  );
}

Token Tokenizer::tokenizeBlockString() {
  this->advanceCursor(3);

  size_t end = this->cursor;
  while(end < this->source.length()) {
    if(
      this->source[end]     == 0x0022 &&
      this->source[end + 1] == 0x0022 &&
      this->source[end + 2] == 0x0022
    ) {
      break;
    }
    ++end;
  }
  std::string value =
    this->source.substr(this->cursor, end - this->cursor);
  cursor = end + 3;
  return generateToken(
    TokenType::BLOCK_STRING,
    value
  );
}

Token Tokenizer::nextToken() {
  size_t* i = &this->cursor;

  for(;*i < this->source.length(); ++*i) {
    if(isDigit(this->source[*i])) {
      return this->tokenizeDigits();
    } else if(isNameStart(this->source[*i])) {
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
      case 0x0040:
        this->advanceCursor(1);
        return generateToken(TokenType::AT);
    }
  }
  return generateToken(TokenType::END_OF_QUERY);
}

void Tokenizer::advanceCursor(int amount) {
  this->cursor += amount;
}

} // cgql
