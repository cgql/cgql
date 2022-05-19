#include "cgql/base/baseParser.h"
#include "cgql/utilities/assert.h"

namespace cgql {
namespace internal {

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
  TokenType currentTokenType = this->tokenizer.current.getType();
  switch(currentTokenType) {
    case TokenType::SQUARE_BRACES_L:
      return this->parseList();
    case TokenType::CURLY_BRACES_L:
      return this->parseObject();
    case TokenType::NAME:
      return this->parseName();
    case TokenType::STRING:
      return this->move(TokenType::STRING).getValue();
    case TokenType::INT: {
      std::string valueAsStr =
        this->move(TokenType::INT).getValue();
      // potentially an integer
      return strToInt<Int>(valueAsStr);
    }
    default:
      cgqlAssert(false, "Unexpected tokentype");
      return 0;
  }
}

Token BaseParser::move(TokenType type) {
  bool isValidType = this->checkType(type);
  if(!isValidType) {
    std::string errorMsg;
    errorMsg += "Required token ";
    errorMsg += tokenTypeToCharArray(type);
    errorMsg += ", but got ";
    errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());

    cgqlAssert(isValidType, errorMsg.c_str());
  }
  Token returnToken = this->tokenizer.current;
  this->tokenizer.advance();
  return returnToken;
}

bool BaseParser::checkType(TokenType type) {
  if(this->tokenizer.current.getType() == type)
    return true;
  return false;
}

std::string BaseParser::parseName() {
  return this->move(TokenType::NAME).getValue();
}

} /* internal */ 
} /* cgql */ 
