#include "cgql/schema/parser.h"

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
    type = registry.getType<TypeDefinition>(this->parseName());
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

cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> SchemaParser::parseImplementInterfaces(const TypeRegistry& registry) {
  if(this->tokenizer.current.getValue() != "implements") return {};
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> interfaces;
  do {
    this->tokenizer.advance();
    interfaces.emplace_back(parseImplementInterface(this->parseName()));
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
  std::string name(this->parseName());
  cgqlSPtr<ObjectTypeDefinition> obj = registry.getType<ObjectTypeDefinition>(name);
  obj->setName(name);
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
  std::string name(this->parseName());
  cgqlSPtr<InterfaceTypeDefinition> interface = registry.getType<InterfaceTypeDefinition>(name);
  interface->setName(name);
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

cgqlSPtr<internal::Schema> parseSchema(const char *source, const TypeRegistry& registry) {
  SchemaParser parser(source);
  return cgqlSMakePtr<internal::Schema>();
}

} /* internal */ 
} /* cgql */ 


