#include "cgql/schema/parser.h"

namespace cgql {
namespace internal {

template<typename T>
cgqlSPtr<T> SchemaParser::parseType() {
  cgqlSPtr<T> type;
  if(this->checkType(TokenType::SQUARE_BRACES_L)) {
    this->tokenizer.advance();
    type = cgqlSMakePtr<ListTypeDefinition<TypeDefinition>>(
      this->parseType<TypeDefinition>()
    );
    this->move(TokenType::SQUARE_BRACES_R);
  } else {
    type = cgqlSMakePtr<T>();
    type->setName(this->parseName());
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

cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> SchemaParser::parseImplementInterfaces() {
  if(this->tokenizer.current.getValue() != "implements") return {};
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> interfaces;
  do {
    this->tokenizer.advance();
    interfaces.emplace_back(parseImplementInterface(this->parseName()));
  } while(this->checkType(TokenType::AMPERSAND));
  return interfaces;
}

ArgumentTypeDefinition SchemaParser::parseArgumentDefinition() {
  std::string name(this->parseName());
  this->move(TokenType::COLON);
  ArgumentTypeDefinition arg;
  cgqlSPtr<TypeDefinition> type = this->parseType();
  arg.setName(name);
  arg.setType(type);
  return arg;
}

FieldTypeDefinition SchemaParser::parseFieldTypeDefinition() {
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

cgqlUPtr<ObjectTypeDefinition> SchemaParser::parseObjectTypeDefinition() {
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

cgqlUPtr<InterfaceTypeDefinition> SchemaParser::parseInterfaceTypeDefinition() {
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

/* cgqlSPtr<internal::Schema> parseSchema(const char *source, const TypeRegistry& registry) {
  SchemaParser parser;
} */

} /* internal */ 
} /* cgql */ 


