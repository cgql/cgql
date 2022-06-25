#pragma once

#include "cgql/schema/GraphQLTypes.h"

#include "cgql/lexer/tokenizer.h"

namespace cgql {
  
class BaseParser {
public:
  BaseParser(const char* document);
protected:
  GraphQLInputTypes parseValueLiteral();

  Token move(TokenType type);
  bool checkType(TokenType type);

  Tokenizer tokenizer;

  std::string parseName();
};

} /* cgql */
