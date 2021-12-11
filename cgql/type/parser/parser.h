#ifndef PARSER_H
#define PARSER_H

#include "../../cgqlPch.h"

#include "../Document.h"
#include "tokenizer.h"

namespace cgql {
namespace internal {

using std::string;

class Parser {
public:
  Parser(const char* document);
  ~Parser();
  Document parseDocument();
private:
  Token move(TokenType type);
  bool moveUntil(TokenType type);
  bool checkType(TokenType type);

  string document;
  Tokenizer tokenizer;

  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  Selection parseSelection();
  Field* parseField();

  string parseName();

  GraphQLScalarTypes parseType();
  GraphQLObject parseObjectTypeDefinition();
  GraphQLField parseFieldTypeDefinition();
};
} // internal

internal::Document parse(const char* source);
} // cgql

#endif
