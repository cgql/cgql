#include "cgql/cgqlPch.h"

#include "cgql/type/parser/documentToSchema.h"
#include "cgql/type/parser/parser.h"
#include "cgql/utilities/assert.h"
#include "cgql/logger/logger.h"
#include "cgql/type/parser/parser.h"

namespace cgql {
namespace internal {

Parser::Parser(const char* document)
  : document(document),
    tokenizer(document) {}
Parser::~Parser() {}

Token Parser::move(const TokenType& type) {
  bool isValidType = this->checkType(type);
  if(!isValidType) {
    std::string errorMsg;
    errorMsg += "Required token ";
    errorMsg += tokenTypeToCharArray(type);
    errorMsg += ", but got ";
    errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());

    cgqlAssert(!isValidType, errorMsg.c_str());
  }
  Token returnToken = this->tokenizer.current;
  this->tokenizer.advance();
  return returnToken;
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

std::string Parser::parseName() {
  std::string name = this->move(TokenType::NAME).getValue();
  return name;
}

Arg Parser::parseValue() {
  std::string valueAsStr;
  TokenType curr = this->tokenizer.current.getType();
  switch(curr) {
    case TokenType::NAME:
      valueAsStr = this->parseName();
      break;
    default:
      valueAsStr = this->move(curr).getValue();
  }
  uint8_t start = charToInt<uint8_t>(valueAsStr[0]);
  if(isAsciiDigit(start)) {
    // potentially an integer
    return strToInt<Int>(valueAsStr);
  }
  return valueAsStr;
}

Argument Parser::parseArgument() {
  Argument argument;
  std::string name = this->parseName();
  this->move(TokenType::COLON);
  Arg value = this->parseValue();
  argument.setName(name);
  argument.setValue(value);
  return argument;
}

cgqlSPtr<Field> Parser::parseField() {
  Field field;
  std::string aliasOrName = this->parseName();
  if(this->checkType(TokenType::COLON)) {
    this->tokenizer.advance();
    std::string name = this->parseName();
    field.setAlias(aliasOrName); // alias
    field.setName(name);
  } else {
    field.setName(aliasOrName);
  }

  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArgs(
        this->parseArgument()
      );
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }

  bool hasSelectionSet = this->checkType(TokenType::CURLY_BRACES_L);
  if(hasSelectionSet) {
    SelectionSet selections = this->parseSelectionSet();
    field.setSelectionSet(std::move(selections));
    cgqlAssert(
      selections.size() == 0,
      "selectionSet should contain atleast one selection"
    );
  }
  return cgqlSMakePtr<Field>(field);
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

Type Parser::parseType() {
  Type type;
  if(this->checkType(TokenType::SQUARE_BRACES_L)) {
    this->tokenizer.advance();
    Type innerType = this->parseType();
    this->move(TokenType::SQUARE_BRACES_R);
    type.setTypeList(true);
    type.setWrappedInnerType(innerType);
  } else {
    type.setName(this->parseName());
  }
  if(this->checkType(TokenType::BANG)) {
    this->tokenizer.advance();
    Type nonNullType;
    nonNullType.setWrappedInnerType(type);
    nonNullType.setTypeNonNull(true);
    return nonNullType;
  }
  return type;
}

ArgumentDefinitions Parser::parseArgumentDefinition() {
  std::string name = this->parseName();
  this->move(TokenType::COLON);
  ArgumentDefinitions arg;
  Type type = this->parseType();
  arg.setName(name);
  arg.setType(type);
  return arg;
}

FieldDefinition Parser::parseFieldTypeDefinition() {
  FieldDefinition field;
  std::string name = this->parseName();
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArg(this->parseArgumentDefinition());
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }
  this->move(TokenType::COLON);
  Type type = this->parseType();
  field.setName(name);
  field.setType(type);
  return field;
}

ObjectTypeDefinition Parser::parseObjectTypeDefinition() {
  this->tokenizer.advance();
  std::string name = this->parseName();
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
  Definition definition;
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    definition = this->parseOperationDefinition();
  } else if(this->checkType(TokenType::NAME)) {
    std::string currentValue =
      this->tokenizer.current.getValue();
    if(currentValue == "type") {
      definition = this->parseObjectTypeDefinition();
    }
  } else {
    std::string errorMsg;
    errorMsg += "Unexpected token ";
    errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());
    cgqlAssert(false, errorMsg.c_str());
  }
  return definition;
}

Document Parser::parseDocument() {
  cgqlContainer<Definition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.push_back(this->parseDefinition());
  } while (!this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};

GraphQLSchema documentToSchema(const internal::Document& doc) {
  std::unordered_map<Type, TypeDefinition> typeMap;
  for(auto const& def : doc.getDefinitions()) {
    if(def.index() == 1) {
      TypeDefinition objDef =
        fromVariant<TypeDefinition>(def);
      AbstractTypeDefinition abstractTypeDef =
        fromVariant<ObjectTypeDefinition>(objDef);
      typeMap.try_emplace(
        abstractTypeDef.getName(),
        objDef
      );
    }
  }

  DocToSchemaParser docToSchemaParser;
  return docToSchemaParser.docToSchemaImpl(typeMap);
}

} // internal

internal::Document parse(const char *document) {
  internal::Parser parser(document);
  internal::Document doc = parser.parseDocument();
  return doc;
};

GraphQLSchema parseSchema(const char *source) {
  internal::Parser parser(source);
  internal::Document doc = parser.parseDocument();
  GraphQLSchema schema = internal::documentToSchema(doc);
  return schema;
}

} // cgql
