#include "cgql/parser/parser.h"
#include "cgql/logger/logger.h"

#include <cassert>

namespace cgql {

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
    field->setAlias(aliasOrName); // alias
    field->setName(this->parseName());
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

  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    field->setSelectionSet(this->parseSelectionSet());
  }
  return field;
}

cgqlSPtr<InlineFragment> QueryParser::parseInlineFragment() {
  cgqlSPtr<InlineFragment> inlineFragment =
    cgqlSMakePtr<InlineFragment>();

  this->tokenizer.advance();
  inlineFragment->setTypeCondition(this->parseName());
  inlineFragment->setSelectionSet(this->parseSelectionSet());
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
  if(this->tokenizer.current.value != "on") {}
  this->tokenizer.advance();
  fragment.setTypeCondition(this->parseName());
  fragment.setSelectionSet(this->parseSelectionSet());
  return fragment;
}

cgqlSPtr<Selection> QueryParser::parseSelection() {
  if(this->checkType(TokenType::SPREAD)) {
    this->tokenizer.advance();
    if(this->tokenizer.current.value == "on")
      return this->parseInlineFragment();
    return this->parseFragment();
  }
  return this->parseField();
}

SelectionSet QueryParser::parseSelectionSet() {
  this->move(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  do {
    selections.push_back(this->parseSelection());
  } while(!this->checkType(TokenType::CURLY_BRACES_R));
  this->tokenizer.advance();
  return selections;
}

OperationDefinition QueryParser::parseOperationDefinition() {
  return {
    this->parseSelectionSet(),
    OperationType::QUERY
  };
}

Definition QueryParser::parseDefinition() {
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    return this->parseOperationDefinition();
  }
  String lookAhead(this->tokenizer.current.value);
  if(lookAhead == "fragment") return this->parseFragmentDefinition();
  assert(false && "Unexpected token");
  /* silence compiler warning */ return {};
}

Document QueryParser::parse() {
  cgqlContainer<Definition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.push_back(this->parseDefinition());
  } while (!this->checkType(TokenType::END_OF_QUERY));
  return Document(definitions);
};


Document parse(const char *document) {
  QueryParser parser(document);
  return parser.parse();
};

} // cgql
