#ifndef PARSER_H
#define PARSER_H

#include "cgql/base/cgqlPch.h"

#include "cgql/cgql.h"
#include "cgql/type/Document.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/type/parser/tokenizer.h"
#include "cgql/base/baseParser.h"

namespace cgql {
namespace internal {

class QueryParser : BaseParser {
public:
  using BaseParser::BaseParser;

  Document parse();
private:
  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  cgqlUPtr<Selection> parseSelection();
  cgqlUPtr<Field> parseField();
  cgqlUPtr<InlineFragment> parseInlineFragment();
  Argument parseArgument();
  Arg parseValue();
};

} // internal

internal::Document parse(const char* source);
} // cgql

#endif
