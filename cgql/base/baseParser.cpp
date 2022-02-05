#include "cgql/base/baseParser.h"
#include "cgql/utilities/assert.h"

namespace cgql {
namespace internal {

BaseParser::BaseParser(const char* document) 
  : tokenizer(document) {
}

Token BaseParser::move(TokenType type) {
  bool isValidType = this->checkType(type);
  if(!isValidType) {
    std::string errorMsg;
    errorMsg += "Required token ";
    errorMsg += tokenTypeToCharArray(type);
    errorMsg += ", but got ";
    errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());

    cgqlAssert(!isValidType, errorMsg.c_str());
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
