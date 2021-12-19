#include "../../cgqlPch.h"

#include "cgql/type/parser/documentToSchema.h"
#include "parser.h"
#include "../../logger/logger.h"

namespace cgql {
namespace internal {

using std::vector;

Parser::Parser(const char* document)
  : document(document),
    tokenizer(document) {}
Parser::~Parser() {}

Token Parser::move(const TokenType& type) {
  if(checkType(type)) {
    Token returnToken = this->tokenizer.current;
    this->tokenizer.advance();
    return returnToken;
  }
  logger::error(type);
  logger::error(this->tokenizer.current.getType());
  throw new InvalidTokenType(type, this->tokenizer.current.getType());
}

bool Parser::moveUntil(const TokenType& type) {
  if(!this->checkType(type)) {
    this->tokenizer.advance();
    return true;
  }
  return false;
}

bool Parser::checkType(const TokenType& type) {
  if(this->tokenizer.current.getType() == type)
    return true;
  return false;
}

string Parser::parseName() {
  string name = this->move(TokenType::NAME).getValue();
  return name;
}

Field* Parser::parseField() {
  string name = this->parseName();
  bool hasSelectionSet = this->checkType(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  if(hasSelectionSet) {
    selections = this->parseSelectionSet();
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
  this->tokenizer.advance();
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

string Parser::parseType() {
  Token type = this->move(TokenType::NAME);
  string name = type.getValue();
  if(name == "String") {
    return "String";
  } else if(name == "Int") {
    return "Int";
  } else {
    // the type should be an object by now
    return name.c_str();
  }
}

FieldDefinition Parser::parseFieldTypeDefinition() {
  string name = this->parseName();
  this->move(TokenType::COLON);
  string type = this->parseType();
  FieldDefinition field;
  field.setName(name);
  field.setType(type);
  return field;
}

ObjectTypeDefinition Parser::parseObjectTypeDefinition() {
  this->tokenizer.advance();
  string name = this->parseName();
  ObjectTypeDefinition obj;
  obj.setName(name);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      obj.addField(
        this->parseFieldTypeDefinition()
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
  return obj;
}

Definition Parser::parseDefinition() {
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    return this->parseOperationDefinition();
  }

  if(this->checkType(TokenType::NAME)) {
    string currentValue =
      this->tokenizer.current.getValue();
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
  } while (!this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};

GraphQLSchema documentToSchema(const internal::Document& doc) {
  std::unordered_map<std::string, TypeDefinition> typeMap;
  for(auto def : doc.getDefinitions()) {
    if(def.index() == 1) {
      TypeDefinition objDef =
        std::get<TypeDefinition>(def);
      AbstractTypeDefinition abstractTypeDef =
        std::get<ObjectTypeDefinition>(objDef);
      typeMap.insert({
        abstractTypeDef.getName(),
        objDef
      });
    }
  }

  DocToSchemaParser docToSchemaParser;
  return docToSchemaParser.docToSchemaImpl(typeMap);
}

} // internal

internal::Document parse(const char *document) {
  internal::Parser parser(document);
  return parser.parseDocument();
};

GraphQLSchema parseSchema(const char *source) {
  internal::Parser parser(source);
  internal::Document doc = parser.parseDocument();
  return internal::documentToSchema(doc);
}

} // cgql
