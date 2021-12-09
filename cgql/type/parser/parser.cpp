#include "../../cgqlPch.h"

#include "parser.h"
#include "../../logger/logger.h"

namespace cgql {
namespace internal {

using std::vector;

Parser::Parser(const char* document)
  : document(document),
    tokenizer(this->document) {}
Parser::~Parser() {}

Token Parser::move(TokenType type) {
  if(checkType(type)) {
    Token returnToken = this->tokenizer.current;
    this->tokenizer.advance();
    return returnToken;
  }
  logger::error(type);
  logger::error(this->tokenizer.current.getType());
  throw new InvalidTokenType(type, this->tokenizer.current.getType());
}

bool Parser::moveUntil(TokenType type) {
  if(!this->checkType(type)) {
    this->tokenizer.advance();
    return true;
  }
  return false;
}

bool Parser::checkType(TokenType type) {
  if(this->tokenizer.current.getType() == type)
    return true;
  return false;
}

string Parser::parseName() {
  return this->move(TokenType::NAME).getValue();
}

Field* Parser::parseField() {
  string name = this->parseName();
  bool hasSelectionSet = this->checkType(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  if(hasSelectionSet) {
    selections = this->parseSelectionSet();
    this->tokenizer.advance();
  }
  Field* field = new Field(
    name,
    selections
  );
  return field;
}

Selection Parser::parseSelection() {
  return this->parseField();
}

SelectionSet Parser::parseSelectionSet() {
  this->move(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  do {
    selections.push_back(this->parseSelection());
  } while(!this->checkType(TokenType::CURLY_BRACES_R));
  return {
    selections
  };
}

OperationDefinition Parser::parseOperationDefinition() {
  return {
    OperationType::QUERY,
    this->parseSelectionSet()
  };
}

Definition Parser::parseDefinition() {
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    return this->parseOperationDefinition();
  }

  throw this->tokenizer.current;
}

Document Parser::parseDocument() {
  vector<Definition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.push_back(this->parseDefinition());
  } while (this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};
} // internal

internal::Document parse(const char *document) {
  internal::Parser parser(document);
  return parser.parseDocument();
};
} // cgql
