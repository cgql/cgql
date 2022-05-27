#pragma once

#include "cgql/cgql.h"
#include "cgql/query/Document.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/parser/baseParser.h"

#include "cgql/lexer/tokenizer.h"

namespace cgql {
namespace internal {

class QueryParser : BaseParser {
public:
  using BaseParser::BaseParser;

  Document parse();
private:
  Definition parseDefinition();
  OperationDefinition parseOperationDefinition();
  FragmentDefinition parseFragmentDefinition();
  SelectionSet parseSelectionSet();
  cgqlSPtr<Selection> parseSelection();
  cgqlSPtr<Field> parseField();
  cgqlSPtr<InlineFragment> parseInlineFragment();
  cgqlSPtr<Fragment> parseFragment();
  Argument parseArgument();
};

} // internal

internal::Document parse(const char* source);
} // cgql
