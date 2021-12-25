#ifndef PARSER_H
#define PARSER_H

#include "../../cgqlPch.h"

#include "../Document.h"
#include "cgql/schema/GraphQLObject/GraphQLField.h"
#include "cgql/schema/GraphQLTypes.h"
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
  Token move(const TokenType& type);
  bool moveUntil(const TokenType& type);
  bool checkType(const TokenType& type);

  string document;
  Tokenizer tokenizer;

  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  Selection parseSelection();
  cgqlSPtr<Field> parseField();
  Argument parseArgument();
  Arg parseValue();

  string parseName();

  string parseType();
  ObjectTypeDefinition parseObjectTypeDefinition();
  FieldDefinition parseFieldTypeDefinition();
  ArgumentDefinitions parseArgumentDefinition();
};
} // internal

internal::Document parse(const char* source);
GraphQLSchema parseSchema(const char* source);
} // cgql

#endif
