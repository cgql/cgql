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
    name.data(),
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

GraphQLScalarTypes Parser::parseType() {
  Token type = this->move(TokenType::NAME);
  string name = type.getValue();
  if(name == "String") {
    return GraphQLTypes::GraphQLString;
  } else if(name == "Int") {
    return GraphQLTypes::GraphQLInt;
  } else {
    // the type should be an object by now
    return GraphQLTypes::GraphQLObjectType;
  }
}

GraphQLField Parser::parseFieldTypeDefinition() {
  string name = this->parseName();
  this->move(TokenType::COLON);
  GraphQLScalarTypes type = this->parseType();
  return {
    name,
    type,
    {}
  };
}

GraphQLObject Parser::parseObjectTypeDefinition() {
  string name = this->parseName();
  vector<GraphQLField> fields;
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      fields.push_back(
        this->parseFieldTypeDefinition()
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  return {
    name.data(),
    fields
  };
}

Definition Parser::parseDefinition() {
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    return this->parseOperationDefinition();
  }

  if(this->checkType(TokenType::NAME)) {
    string currentValue =
      this->tokenizer.current.getValue();
    this->tokenizer.advance();
    if(currentValue == "type") {
      return this->parseObjectTypeDefinition();
    }
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
