#ifndef PARSER_H
#define PARSER_H

#include "cgql/base/cgqlPch.h"

#include "cgql/type/Document.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/type/parser/tokenizer.h"

namespace cgql {
namespace internal {

class Parser {
public:
  Parser(const char* document);
  ~Parser();
  Document parseDocument();
private:
  Token move(const TokenType& type);
  bool moveUntil(const TokenType& type);
  bool checkType(const TokenType& type);

  std::string document;
  Tokenizer tokenizer;

  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  Selection parseSelection();
  cgqlSPtr<Field> parseField();
  Argument parseArgument();
  Arg parseValue();

  std::string parseName();

  cgqlSPtr<TypeDefinition> parseType();
  cgqlSPtr<ObjectTypeDefinition> parseObjectTypeDefinition();
  FieldTypeDefinition parseFieldTypeDefinition();
  ArgumentTypeDefinition parseArgumentDefinition();
};
} // internal

internal::Document parse(const char* source);
cgqlSPtr<internal::Schema> parseSchema(const char* source);
} // cgql

#endif
