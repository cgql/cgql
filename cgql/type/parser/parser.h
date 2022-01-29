#ifndef PARSER_H
#define PARSER_H

#include "cgql/base/cgqlPch.h"

#include "cgql/cgql.h"
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
  Token move(TokenType type);
  bool checkType(TokenType type);

  Tokenizer tokenizer;

  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  cgqlUPtr<Selection> parseSelection();
  cgqlUPtr<Field> parseField();
  cgqlUPtr<InlineFragment> parseInlineFragment();
  Argument parseArgument();
  Arg parseValue();

  std::string parseName();

  template<typename T = TypeDefinition>
  cgqlSPtr<T> parseType();
  cgqlUPtr<ObjectTypeDefinition> parseObjectTypeDefinition();
  cgqlUPtr<InterfaceTypeDefinition> parseInterfaceTypeDefinition();
  FieldTypeDefinition parseFieldTypeDefinition();
  ArgumentTypeDefinition parseArgumentDefinition();
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> parseImplementInterfaces();
};

cgqlSPtr<Schema> parseSchema(const char* source, const TypeRegistry& registry);

} // internal

internal::Document parse(const char* source);
} // cgql

#endif
