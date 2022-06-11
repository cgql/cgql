#include "cgql/lexer/tokenizer.h"
#include "cgql/logger/logger.h"

namespace cgql {

static inline Token generateToken(TokenType type) {
  return Token {
    .type = type
  };
}
static inline Token generateToken(TokenType type, std::string value) {
  return Token {
    .value = value,
    .type = type
  };
}


static inline bool isLetter(uint32_t value) {
  return (value >= 0x0041 && value <= 0x005A) ||
         (value >= 0x0061 && value <= 0x007A);
}

static inline bool isDigit(uint32_t value) {
   return value >= 0x0030 && value <= 0x0039;
}

static inline bool isNameStart(uint32_t value) {
  return isLetter(value) || value == 0x005F;
}

static inline bool isNameContinue(uint32_t value) {
  return isLetter(value) || isDigit(value) || value == 0x005F;
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
    switch (this->source[*i]) {
      case ' ':
      case '\n':
      case '\t':
      case ',':
        continue;
      case '{':
        this->advanceCursor(1);
        return generateToken(TokenType::CURLY_BRACES_L);
      case '}':
        this->advanceCursor(1);
        return generateToken(TokenType::CURLY_BRACES_R);
      case '[':
        this->advanceCursor(1);
        return generateToken(TokenType::SQUARE_BRACES_L);
      case ']':
        this->advanceCursor(1);
        return generateToken(TokenType::SQUARE_BRACES_R);
      case '(':
        this->advanceCursor(1);
        return generateToken(TokenType::BRACES_L);
      case ')':
        this->advanceCursor(1);
        return generateToken(TokenType::BRACES_R);
      case ':':
        this->advanceCursor(1);
        return generateToken(TokenType::COLON);
      case '!':
        this->advanceCursor(1);
        return generateToken(TokenType::BANG);
      case '&':
        this->advanceCursor(1);
        return generateToken(TokenType::AMPERSAND);
      case '.':
        if(
          this->source[*i + 1] == '.' &&
          this->source[*i + 2] == '.'
        ) {
          this->advanceCursor(3);
          return generateToken(TokenType::SPREAD);
        }
      case '=':
        this->advanceCursor(1);
        return generateToken(TokenType::EQUAL);
      case '|':
        this->advanceCursor(1);
        return generateToken(TokenType::PIPE);
      case '#':
        this->advanceCursor(1);
        this->skipComments();
        continue;
      case '"':
        if(
          this->source[*i + 1] == '"' &&
          this->source[*i + 2] == '"'
        ) {
          return this->tokenizeBlockString();
        }
        return this->tokenizeString();
      case '@':
        this->advanceCursor(1);
        return generateToken(TokenType::AT);
      default:
        // Unicode BOM
        if(static_cast<uint32_t>(this->source[*i]) == 0xFEFF)
          continue;
    }
  }
  return generateToken(TokenType::END_OF_QUERY);
}

void Tokenizer::advanceCursor(int amount) {
  this->cursor += amount;
}

} // cgql
