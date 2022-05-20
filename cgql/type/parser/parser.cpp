#include "cgql/base/cgqlPch.h"

#include "cgql/type/parser/parser.h"
#include "cgql/utilities/assert.h"
#include "cgql/logger/logger.h"
#include "cgql/type/parser/parser.h"

namespace cgql {
namespace internal {

Argument QueryParser::parseArgument() {
  Argument argument;
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  GraphQLInputTypes value = this->parseValueLiteral();
  argument.setName(name);
  argument.setValue(value);
  return argument;
}

cgqlSPtr<Field> QueryParser::parseField() {
  cgqlSPtr<Field> field = cgqlSMakePtr<Field>();
  std::string aliasOrName(this->parseName());
  if(!this->checkType(TokenType::COLON)) {
    field->setName(aliasOrName);
  } else {
    this->tokenizer.advance();
    std::string name(this->parseName());
    field->setAlias(aliasOrName); // alias
    field->setName(name);
  }

  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field->addArgs(
        this->parseArgument()
      );
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }

  bool hasSelectionSet = this->checkType(TokenType::CURLY_BRACES_L);
  if(hasSelectionSet) {
    SelectionSet selections = this->parseSelectionSet();
    field->setSelectionSet(selections);
  }
  return field;
}

cgqlSPtr<InlineFragment> QueryParser::parseInlineFragment() {
  cgqlSPtr<InlineFragment> inlineFragment =
    cgqlSMakePtr<InlineFragment>();
  // TODO: handle error
  if(this->tokenizer.current.getValue() != "on") {}
  this->tokenizer.advance();
  inlineFragment->setTypeCondition(this->parseName());
  SelectionSet selectionSet = this->parseSelectionSet();
  inlineFragment->setSelectionSet(selectionSet);
  return inlineFragment;
}

cgqlSPtr<Fragment> QueryParser::parseFragment() {
  cgqlSPtr<Fragment> fragment =
    cgqlSMakePtr<Fragment>();
  fragment->setName(this->parseName());
  return fragment;
}

FragmentDefinition QueryParser::parseFragmentDefinition() {
  FragmentDefinition fragment;
  this->tokenizer.advance();
  fragment.setName(this->parseName());
  // TODO: handle error
  if(this->tokenizer.current.getValue() != "on") {}
  this->tokenizer.advance();
  fragment.setTypeCondition(this->parseName());
  SelectionSet selectionSet = this->parseSelectionSet();
  fragment.setSelectionSet(selectionSet);
  return fragment;
}

cgqlSPtr<Selection> QueryParser::parseSelection() {
  if(this->checkType(TokenType::SPREAD)) {
    this->tokenizer.advance();
    if(this->tokenizer.current.getValue() == "on")
      return this->parseInlineFragment();
    return this->parseFragment();
  }
  return this->parseField();
}

SelectionSet QueryParser::parseSelectionSet() {
  this->move(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  do {
    selections.emplace_back((this->parseSelection()));
  } while(!this->checkType(TokenType::CURLY_BRACES_R));
  this->tokenizer.advance();
  return selections;
}

OperationDefinition QueryParser::parseOperationDefinition() {
  return {
    OperationType::QUERY,
    this->parseSelectionSet()
  };
}

Definition QueryParser::parseDefinition() {
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    return this->parseOperationDefinition();
  }
  String lookAhead(this->tokenizer.current.getValue());
  if(lookAhead == "fragment") return this->parseFragmentDefinition();
  std::string errorMsg;
  errorMsg += "Unexpected token ";
  errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());
  cgqlAssert(false, errorMsg.c_str());
  /* silence compiler warning */ return {};
}

Document QueryParser::parse() {
  cgqlContainer<Definition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.emplace_back(this->parseDefinition());
  } while (!this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};

} // internal

internal::Document parse(const char *document) {
  internal::QueryParser parser(document);
  return parser.parse();
};

} // cgql
