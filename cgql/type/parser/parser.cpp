#include "parser.h"
#include "../../logger/logger.h"
#include <vector>

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
  Field* field = new Field(
    this->parseName()
  );
  return field;
}

Selection Parser::parseSelection() {
  return this->parseField();
}

SelectionSet Parser::parseSelectionSet() {
  this->move(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  while(!this->checkType(TokenType::CURLY_BRACES_R)) {
    selections.push_back(this->parseSelection());
  }
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

Document Parser::parseDocument() {
  vector<OperationDefinition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.push_back(this->parseOperationDefinition());
  } while (this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};

Document parse(const char *document) {
  Parser parser(document);
  return parser.parseDocument();
};
