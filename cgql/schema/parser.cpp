#include "cgql/schema/parser.h"
#include "cgql/schema/typeDefinitions.hpp"

namespace cgql {
namespace internal {

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

cgqlContainer<std::string> SchemaParser::parseImplementInterfaces(const TypeRegistry& registry) {
  if(this->tokenizer.current.getValue() != "implements") return {};
  cgqlContainer<std::string> interfaces;
  do {
    this->tokenizer.advance();
    interfaces.emplace_back(this->parseName());
  } while(this->checkType(TokenType::AMPERSAND));
  return interfaces;
}

ArgumentTypeDefinition SchemaParser::parseArgumentDefinition(const TypeRegistry& registry) {
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  ArgumentTypeDefinition arg;
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  arg.setName(name);
  arg.setType(type);
  return arg;
}

FieldTypeDefinition SchemaParser::parseFieldTypeDefinition(const TypeRegistry& registry) {
  FieldTypeDefinition field;
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
  field.setType(type);
  return field;
}

void SchemaParser::parseObjectTypeDefinition(const TypeRegistry& registry) {
  this->tokenizer.advance();
  cgqlSPtr<ObjectTypeDefinition> obj =
    registry.getType<ObjectTypeDefinition>(this->parseName());
  obj->setImplementedInterfaces(this->parseImplementInterfaces(registry));
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
  this->tokenizer.advance();
  cgqlSPtr<InterfaceTypeDefinition> interface =
    registry.getType<InterfaceTypeDefinition>(this->parseName());
  interface->setImplementedInterfaces(this->parseImplementInterfaces(registry));
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
  this->tokenizer.advance();
  cgqlSPtr<UnionTypeDefinition> unionType =
    registry.getType<UnionTypeDefinition>(this->parseName());
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
  this->tokenizer.advance();
  cgqlSPtr<EnumTypeDefinition> enumType =
    registry.getType<EnumTypeDefinition>(this->parseName());
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      enumType->addValue(this->parseName());
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
  }
  this->tokenizer.advance();
}

InputValueDefinition SchemaParser::parseInputValueDefinition(const TypeRegistry& registry) {
  InputValueDefinition field;
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  field.setName(name);
  field.setType(type);
  return field;
}

void SchemaParser::parseInputObjectTypeDefinition(const TypeRegistry& registry) {
  this->tokenizer.advance();
  cgqlSPtr<InputObjectTypeDefinition> inputType =
    registry.getType<InputObjectTypeDefinition>(this->parseName());
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
  this->tokenizer.advance();
  cgqlSPtr<ScalarTypeDefinition> scalar =
    registry.getType<ScalarTypeDefinition>(this->parseName());
}

void SchemaParser::parseDefinition(const TypeRegistry& registry) {
  String currentValue(this->tokenizer.current.getValue());
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
    cgqlAssert(true, msg.c_str());
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


