#pragma once

#include "cgql/query/Document.h"
#include "cgql/parser/baseParser.h"

namespace cgql {

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

Document parse(const char* source);
} // cgql
