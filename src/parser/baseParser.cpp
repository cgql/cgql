#include "cgql/parser/baseParser.h"

#include <cassert>

namespace cgql {

BaseParser::BaseParser(const char* document) 
  : tokenizer(document) {
}

cgqlSPtr<ObjectType> BaseParser::parseObject() {
  this->tokenizer.advance();
  cgqlSPtr<ObjectType> object = cgqlSMakePtr<ObjectType>();
  while(!this->checkType(TokenType::CURLY_BRACES_R)) {
    std::string key = this->parseName();
    this->move(TokenType::COLON);
    object->fields.try_emplace(key, this->parseValueLiteral());
  }
  this->tokenizer.advance();
  return object;
}

cgqlSPtr<ListType> BaseParser::parseList() {
  this->tokenizer.advance();
  cgqlSPtr<ListType> list = cgqlSMakePtr<ListType>();
  while(!this->checkType(TokenType::SQUARE_BRACES_R)) {
    list->elements.emplace_back(this->parseValueLiteral());
  }
  this->tokenizer.advance();
  return list;
}

GraphQLInputTypes BaseParser::parseValueLiteral() {
  TokenType currentTokenType = this->tokenizer.current.type;
  switch(currentTokenType) {
    case TokenType::NAME:
      return this->parseName();
    case TokenType::INT: {
      // potentially an integer
      return std::stoi(
        this->move(TokenType::INT).value
      );
    }
    case TokenType::STRING:
      return this->move(TokenType::STRING).value;
    case TokenType::CURLY_BRACES_L:
      return this->parseObject();
    case TokenType::SQUARE_BRACES_L:
      return this->parseList();
    default:
      assert(false && "Unexpected tokentype");
      return 0;
  }
}

Token BaseParser::move(TokenType type) {
  assert(this->checkType(type) && "Invalid tokentype");
  Token returnToken = this->tokenizer.current;
  this->tokenizer.advance();
  return returnToken;
}

bool BaseParser::checkType(TokenType type) {
  return this->tokenizer.current.type == type;
}

std::string BaseParser::parseName() {
  return this->move(TokenType::NAME).value;
}

} /* cgql */ 
