#include "cgql/base/cgqlPch.h"

#include "cgql/type/parser/parser.h"
#include "cgql/type/parser/docToSchema.h"
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
  return this->move(TokenType::NAME).getValue();
}

Arg Parser::parseValue() {
  TokenType curr = this->tokenizer.current.getType();
  std::string valueAsStr = [&curr, this](){;
    switch(curr) {
      case TokenType::NAME:
        return this->parseName();
        break;
      default:
        return this->move(curr).getValue();
    }
  }();
  uint8_t start = charToInt<uint8_t>(valueAsStr[0]);
  if(isAsciiDigit(start)) {
    // potentially an integer
    return strToInt<Int>(valueAsStr);
  }
  return valueAsStr;
}

Argument Parser::parseArgument() {
  Argument argument;
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  Arg value = this->parseValue();
  argument.setName(name);
  argument.setValue(value);
  return argument;
}

cgqlUPtr<Field> Parser::parseField() {
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

cgqlUPtr<InlineFragment> Parser::parseInlineFragment() {
  cgqlUPtr<InlineFragment> inlineFragment =
    cgqlUMakePtr<InlineFragment>();
  this->tokenizer.advance();
  // TODO: handle error
  if(this->tokenizer.advance().getValue() != "on") {}
  inlineFragment->setTypeCondition(this->parseName());
  SelectionSet selectionSet = this->parseSelectionSet();
  inlineFragment->setSelectionSet(selectionSet);
  return inlineFragment;
}

cgqlUPtr<Selection> Parser::parseSelection() {
  if(this->checkType(TokenType::SPREAD)) {
    return this->parseInlineFragment();
  }
  return this->parseField();
}

SelectionSet Parser::parseSelectionSet() {
  this->move(TokenType::CURLY_BRACES_L);
  SelectionSet selections;
  do {
    selections.emplace_back(this->parseSelection());
  } while(!this->checkType(TokenType::CURLY_BRACES_R));
  this->tokenizer.advance();
  return selections;
}

OperationDefinition Parser::parseOperationDefinition() {
  return {
    OperationType::QUERY,
    this->parseSelectionSet()
  };
}

cgqlSPtr<TypeDefinition> Parser::parseType() {
  cgqlSPtr<TypeDefinition> type;
  if(this->checkType(TokenType::SQUARE_BRACES_L)) {
    this->tokenizer.advance();
    type = cgqlUMakePtr<ListTypeDefinition<TypeDefinition>>(
      this->parseType()
    );
    this->move(TokenType::SQUARE_BRACES_R);
  } else {
    type = cgqlUMakePtr<TypeDefinition>();
    std::string name(this->parseName());
    if(name == "Int") type->setEnumType(DefinitionType::INT_TYPE);
    else if(name == "String") type->setEnumType(DefinitionType::STRING_TYPE);
    type->setName(name);
  }
  if(this->checkType(TokenType::BANG)) {
    this->tokenizer.advance();
    return cgqlSMakePtr<NonNullTypeDefinition<TypeDefinition>>(type);
  }
  return type;
}

static cgqlUPtr<InterfaceTypeDefinition> parseImplementInterface(
  std::string name
) {
  cgqlUPtr<InterfaceTypeDefinition> interface =
    cgqlUMakePtr<InterfaceTypeDefinition>();
  interface->setName(name);
  return interface;
}

cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> Parser::parseImplementInterfaces() {
  if(this->tokenizer.current.getValue() != "implements") return {};
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> interfaces;
  do {
    this->tokenizer.advance();
    interfaces.emplace_back(parseImplementInterface(this->parseName()));
  } while(this->checkType(TokenType::AMPERSAND));
  return interfaces;
}

ArgumentTypeDefinition Parser::parseArgumentDefinition() {
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  ArgumentTypeDefinition arg;
  cgqlSPtr<TypeDefinition> type = this->parseType();
  arg.setName(name);
  arg.setType(type);
  return arg;
}

FieldTypeDefinition Parser::parseFieldTypeDefinition() {
  FieldTypeDefinition field;
  std::string name(this->parseName());
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArg(this->parseArgumentDefinition());
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }
  this->move(TokenType::COLON);
  cgqlSPtr<TypeDefinition> type = this->parseType();
  field.setName(name);
  field.setType(type);
  return field;
}

cgqlUPtr<ObjectTypeDefinition> Parser::parseObjectTypeDefinition() {
  this->tokenizer.advance();
  std::string name(this->parseName());
  cgqlUPtr<ObjectTypeDefinition> obj =
    cgqlUMakePtr<ObjectTypeDefinition>();
  obj->setName(name);
  obj->setImplementedInterfaces(this->parseImplementInterfaces());
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      obj->addField(
        this->parseFieldTypeDefinition()
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
  return obj;
}

cgqlUPtr<InterfaceTypeDefinition> Parser::parseInterfaceTypeDefinition() {
  this->tokenizer.advance();
  std::string name(this->parseName());
  cgqlUPtr<InterfaceTypeDefinition> interface =
    cgqlUMakePtr<InterfaceTypeDefinition>();
  interface->setName(name);
  interface->setImplementedInterfaces(this->parseImplementInterfaces());
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      interface->addField(
        this->parseFieldTypeDefinition()
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
  return interface;
}

Definition Parser::parseDefinition() {
  const Definition definition = [this]() -> Definition {;
    if(this->checkType(TokenType::CURLY_BRACES_L)) {
      return this->parseOperationDefinition();
    } else if(this->checkType(TokenType::NAME)) {
      String currentValue(this->tokenizer.current.getValue());
      if(currentValue == "type")
        return this->parseObjectTypeDefinition();
      else if(currentValue == "interface")
        return this->parseInterfaceTypeDefinition();
    } else {
      std::string errorMsg;
      errorMsg += "Unexpected token ";
      errorMsg += tokenTypeToCharArray(this->tokenizer.current.getType());
      cgqlAssert(false, errorMsg.c_str());
    }
    return nullptr;
  }();
  return definition;
}

Document Parser::parseDocument() {
  cgqlContainer<Definition> definitions;
  this->move(TokenType::START_OF_QUERY);
  do {
    definitions.emplace_back(this->parseDefinition());
  } while (!this->checkType(TokenType::END_OF_QUERY));
  return {
    definitions
  };
};

internal::Schema documentToSchema(Document& doc) {
  Schema schema;
  DocToSchema docToSchema;
  std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeDefMap;
  typeDefMap.reserve(doc.getDefinitions().size());
  for(Definition& def : doc.getDefinitions()) {
    const cgqlSPtr<TypeDefinition>& rootTypeDef =
      fromVariant<cgqlSPtr<TypeDefinition>>(def);
    typeDefMap.try_emplace(rootTypeDef->getName(), rootTypeDef);
  }
  for(Definition& def : doc.getDefinitions()) {
    cgqlSPtr<TypeDefinition> const& rootTypeDef =
      fromVariant<cgqlSPtr<TypeDefinition>>(def);
    DefinitionType const& type = rootTypeDef->getType();
    if(type == DefinitionType::OBJECT_TYPE) {
      cgqlSPtr<ObjectTypeDefinition> objDef =
        std::static_pointer_cast<ObjectTypeDefinition>(rootTypeDef);
      docToSchema.completeObject(objDef, typeDefMap);
      if(rootTypeDef->getName() == "Query") {
        schema.setQuery(objDef);
      }
    } else if(type == DefinitionType::INTERFACE_TYPE) {
      cgqlSPtr<InterfaceTypeDefinition> const& interfaceDef =
        std::static_pointer_cast<InterfaceTypeDefinition>(rootTypeDef);
      docToSchema.completeInterface(interfaceDef, typeDefMap);
    }
  }
  schema.setTypeDefMap(typeDefMap);
  return schema;
}

} // internal

internal::Document parse(const char *document) {
  internal::Parser parser(document);
  internal::Document doc = parser.parseDocument();
  return doc;
};

cgqlSPtr<internal::Schema> parseSchema(const char *source) {
  internal::Parser parser(source);
  internal::Document doc = parser.parseDocument();
  internal::Schema schema = internal::documentToSchema(doc);
  return cgqlSMakePtr<internal::Schema>(schema);
}

} // cgql
