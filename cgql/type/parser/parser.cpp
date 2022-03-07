#include "cgql/base/cgqlPch.h"

#include "cgql/type/parser/parser.h"
#include "cgql/utilities/assert.h"
#include "cgql/logger/logger.h"
#include "cgql/type/parser/parser.h"

namespace cgql {
namespace internal {

std::string QueryParser::getKey() {
  std::string name = this->parseName();
  this->move(TokenType::COLON);
  return name;
}

InputObject QueryParser::parseObject() {
  InputObject inputObject = cgqlSMakePtr<Args>();

  do {
    this->tokenizer.advance();
    inputObject->argsMap.try_emplace(
      this->getKey(), this->parseValue()
    );
  } while(!this->checkType(TokenType::CURLY_BRACES_R));
  this->tokenizer.advance();

  return inputObject;
}

Arg QueryParser::parseValue() {
  TokenType curr = this->tokenizer.current.getType();
  switch(curr) {
    case TokenType::STRING:
      return this->move(curr).getValue();
    case TokenType::INT: {
      std::string valueAsStr = this->move(curr).getValue();
      uint8_t start = charToInt<uint8_t>(valueAsStr[0]);
      if(isAsciiDigit(start)) {
        // potentially an integer
        return strToInt<Int>(valueAsStr);
      }
    }
    case TokenType::CURLY_BRACES_L:
      return this->parseObject();
    default:
      return this->move(curr).getValue();
  }
}

Argument QueryParser::parseArgument() {
  Argument argument;
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  Arg value = this->parseValue();
  argument.setName(name);
  argument.setValue(value);
  return argument;
}

cgqlUPtr<Field> QueryParser::parseField() {
  cgqlUPtr<Field> field = cgqlUMakePtr<Field>();
  std::string aliasOrName(this->parseName());
  if(this->checkType(TokenType::COLON)) {
    this->tokenizer.advance();
    std::string name(this->parseName());
    field->setAlias(aliasOrName); // alias
    field->setName(name);
  } else {
    field->setName(aliasOrName);
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
    cgqlAssert(
      field->getSelectionSet().size() == 0,
      "selectionSet should contain atleast one selection"
    );
  }
  return field;
}

cgqlUPtr<InlineFragment> QueryParser::parseInlineFragment() {
  cgqlUPtr<InlineFragment> inlineFragment =
    cgqlUMakePtr<InlineFragment>();
  // TODO: handle error
  if(this->tokenizer.current.getValue() != "on") {}
  this->tokenizer.advance();
  inlineFragment->setTypeCondition(this->parseName());
  SelectionSet selectionSet = this->parseSelectionSet();
  inlineFragment->setSelectionSet(selectionSet);
  return inlineFragment;
}

cgqlUPtr<Fragment> QueryParser::parseFragment() {
  cgqlUPtr<Fragment> fragment =
    cgqlUMakePtr<Fragment>();
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

cgqlUPtr<Selection> QueryParser::parseSelection() {
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
