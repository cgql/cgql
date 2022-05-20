#pragma once

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
