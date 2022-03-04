#pragma once

#include "cgql/base/cgqlPch.h"
#include "cgql/type/parser/tokenizer.h"

namespace cgql {
namespace internal {
  
class BaseParser {
public:
  BaseParser(const char* document);
protected:
  Token move(TokenType type);
  bool checkType(TokenType type);

  Tokenizer tokenizer;

  std::string parseName();
};

} /* internal */ 
} /* cgql */
