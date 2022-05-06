#include "cgql/schema/parser.h"
#include "cgql/schema/typeDefinitions.hpp"

namespace cgql {
namespace internal {

GraphQLInputTypes SchemaParser::parseValueLiteral() {
  TokenType currentTokenType = this->tokenizer.current.getType();
  switch(currentTokenType) {
    case TokenType::STRING:
      return this->move(currentTokenType).getValue();
    case TokenType::INT: {
      std::string valueAsStr =
        this->move(currentTokenType).getValue();
      // potentially an integer
      return strToInt<Int>(valueAsStr);
    }
    default:
      return this->move(currentTokenType).getValue();
  }
}

std::string SchemaParser::parseDescription() {
  if(
    this->tokenizer.current.getType() == TokenType::STRING ||
    this->tokenizer.current.getType() == TokenType::BLOCK_STRING
  ) {
    std::string description = this->tokenizer.current.getValue();
    this->tokenizer.advance();
    return description;
  }
  return "";
}

template<typename T>
cgqlSPtr<T> SchemaParser::parseType(const TypeRegistry& registry) {
  cgqlSPtr<T> type;
  if(this->checkType(TokenType::SQUARE_BRACES_L)) {
    this->tokenizer.advance();
    type = cgqlSMakePtr<ListTypeDefinition<TypeDefinition>>(
      this->parseType<TypeDefinition>(registry)
    );
    this->move(TokenType::SQUARE_BRACES_R);
  } else {
    type = cgqlSMakePtr<DefaultWrapTypeDefinition<TypeDefinition>>(registry.getType<TypeDefinition>(this->parseName()));
  }
  if(this->checkType(TokenType::BANG)) {
    this->tokenizer.advance();
    return cgqlSMakePtr<NonNullTypeDefinition<TypeDefinition>>(type);
  }
  return type;
}

cgqlContainer<std::string> SchemaParser::parseImplementInterfaces() {
  if(this->tokenizer.current.getValue() != "implements") return {};
  cgqlContainer<std::string> interfaces;
  do {
    this->tokenizer.advance();
    interfaces.emplace_back(this->parseName());
  } while(this->checkType(TokenType::AMPERSAND));
  return interfaces;
}

ArgumentTypeDefinition SchemaParser::parseArgumentDefinition(const TypeRegistry& registry) {
  std::string description(this->parseDescription());
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  ArgumentTypeDefinition arg;
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  if(this->checkType(TokenType::EQUAL)) {
    this->tokenizer.advance();
    arg.setDefaultValue(this->parseValueLiteral());
  }
  arg.setName(name);
  arg.setType(type);
  arg.setDescription(description);
  return arg;
}

FieldTypeDefinition SchemaParser::parseFieldTypeDefinition(const TypeRegistry& registry) {
  FieldTypeDefinition field;
  std::string description(this->parseDescription());
  std::string name(this->parseName());
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArg(this->parseArgumentDefinition(registry));
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }
  this->move(TokenType::COLON);
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  field.setName(name);
  field.setDescription(description);
  field.setType(type);
  return field;
}

void SchemaParser::parseObjectTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<ObjectTypeDefinition> obj =
    registry.getType<ObjectTypeDefinition>(this->parseName());
  obj->setImplementedInterfaces(this->parseImplementInterfaces());
  obj->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      obj->addField(
        this->parseFieldTypeDefinition(registry)
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
}

void SchemaParser::parseInterfaceTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<InterfaceTypeDefinition> interface =
    registry.getType<InterfaceTypeDefinition>(this->parseName());
  interface->setImplementedInterfaces(this->parseImplementInterfaces());
  interface->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      interface->addField(
        this->parseFieldTypeDefinition(registry)
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
}

void SchemaParser::parseUnionTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<UnionTypeDefinition> unionType =
    registry.getType<UnionTypeDefinition>(this->parseName());
  unionType->setDescription(description);
  if(this->checkType(TokenType::EQUAL)) {
    do {
      this->tokenizer.advance();
      unionType->addMember(
        registry.getType<ObjectTypeDefinition>(this->parseName())
      );
    } while(this->checkType(TokenType::PIPE));
  }
}

void SchemaParser::parseEnumTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<EnumTypeDefinition> enumType =
    registry.getType<EnumTypeDefinition>(this->parseName());
  enumType->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      enumType->addValue({ this->parseDescription(), this->parseName() });
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
}

InputValueDefinition SchemaParser::parseInputValueDefinition(const TypeRegistry& registry) {
  InputValueDefinition field;
  std::string description(this->parseDescription());
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  if(this->checkType(TokenType::EQUAL)) {
    this->tokenizer.advance();
    field.setDefaultValue(this->parseValueLiteral());
  }
  field.setName(name);
  field.setType(type);
  field.setDescription(description);
  return field;
}

void SchemaParser::parseInputObjectTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<InputObjectTypeDefinition> inputType =
    registry.getType<InputObjectTypeDefinition>(this->parseName());
  inputType->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      inputType->addField(
        this->parseInputValueDefinition(registry)
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
}

void SchemaParser::parseScalarTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<ScalarTypeDefinition> scalar =
    registry.getType<ScalarTypeDefinition>(this->parseName());
  scalar->setDescription(description);
}

void SchemaParser::parseDefinition(const TypeRegistry& registry) {
  std::string currentValue(this->tokenizer.lookAhead());
  if(currentValue == "type")
    this->parseObjectTypeDefinition(registry);
  else if(currentValue == "interface")
    this->parseInterfaceTypeDefinition(registry);
  else if(currentValue == "union")
    this->parseUnionTypeDefinition(registry);
  else if(currentValue == "enum")
    this->parseEnumTypeDefinition(registry);
  else if(currentValue == "input")
    this->parseInputObjectTypeDefinition(registry);
  else if(currentValue == "scalar")
    this->parseScalarTypeDefinition(registry);
  else {
    std::string msg;
    msg += "Failed to parse type definition which starts with ";
    msg += currentValue.length() ? currentValue : "NULL";
    cgqlAssert(false, msg.c_str());
  }
}

void SchemaParser::parse(const TypeRegistry& registry) {
  this->move(TokenType::START_OF_QUERY);
  do {
    this->parseDefinition(registry);
  } while (!this->checkType(TokenType::END_OF_QUERY));
}

cgqlSPtr<internal::Schema> parseSchema(const char *source, const TypeRegistry& registry) {
  using namespace cgql::internal;
  SchemaParser parser(source);
  parser.parse(registry);
  cgqlSPtr<Schema> schema = cgqlSMakePtr<Schema>();
  schema->setTypeDefMap(registry.getAllTypes());
  return schema;
}

} /* internal */ 
} /* cgql */ 


