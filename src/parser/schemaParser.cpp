#include "cgql/parser/schemaParser.h"
#include "cgql/parser/directiveLocations.h"

namespace cgql {

std::string SchemaParser::parseDescription() {
  if(
    this->tokenizer.current.type == TokenType::BLOCK_STRING ||
    this->tokenizer.current.type == TokenType::STRING
  ) {
    std::string description = this->tokenizer.current.value;
    this->tokenizer.advance();
    return description;
  }
  return "";
}

cgqlSPtr<TypeDefinition> SchemaParser::parseType(const TypeRegistry& registry) {
  cgqlSPtr<TypeDefinition> type;
  if(!this->checkType(TokenType::SQUARE_BRACES_L)) {
    type =
      cgqlSMakePtr<DefaultWrapTypeDefinition>(
        registry.getTypeRef(this->parseName())
      );
  } else {
    this->tokenizer.advance();
    type = cgqlSMakePtr<ListTypeDefinition>(
      this->parseType(registry)
    );
    this->move(TokenType::SQUARE_BRACES_R);
  }
  if(this->checkType(TokenType::BANG)) {
    this->tokenizer.advance();
    return cgqlSMakePtr<NonNullTypeDefinition>(type);
  }
  return type;
}

template<typename T>
void SchemaParser::parseImplementInterfaces(cgqlSPtr<T>& objectOrInterface) {
  if(this->tokenizer.current.value != "implements") return;
  do {
    this->tokenizer.advance();
    objectOrInterface->addImplementedInterface(this->parseName());
  } while(this->checkType(TokenType::AMPERSAND));
}

Directive::DirectiveArgument SchemaParser::parseDirectiveArgument() {
  Directive::DirectiveArgument argument;
  argument.name = this->parseName();
  this->move(TokenType::COLON);
  argument.value = this->parseValueLiteral();
  return argument;
}

cgqlContainer<Directive> SchemaParser::parseDirectives() {
  cgqlContainer<Directive> directives;
  while(this->checkType(TokenType::AT)) {
    this->tokenizer.advance();
    Directive directive;
    directive.setName(this->parseName());
    if(this->checkType(TokenType::BRACES_L)) {
      this->tokenizer.advance();
      do {
        directive.addArgument(this->parseDirectiveArgument());
      } while(!this->checkType(TokenType::BRACES_R));
      this->tokenizer.advance();
    }
    directives.push_back(directive);
  }
  return directives;
}

FieldTypeDefinition SchemaParser::parseFieldTypeDefinition(const TypeRegistry& registry) {
  FieldTypeDefinition field;
  std::string description(this->parseDescription());
  std::string name(this->parseName());
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArg(this->parseInputValueDefinition(registry));
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }
  this->move(TokenType::COLON);
  cgqlSPtr<TypeDefinition> type = this->parseType(registry);
  field.setDirectives(this->parseDirectives());
  field.setName(name);
  field.setDescription(description);
  field.setFieldType(type);
  return field;
}

void SchemaParser::parseObjectTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<ObjectTypeDefinition> obj =
    registry.getType<ObjectTypeDefinition>(this->parseName());
  parseImplementInterfaces(obj);
  obj->setDirectives(this->parseDirectives());
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
  parseImplementInterfaces(interface);
  interface->setDirectives(this->parseDirectives());
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
  unionType->setDirectives(this->parseDirectives());
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
  enumType->setDirectives(this->parseDirectives());
  enumType->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      EnumValueDefinition enumValue(
        this->parseDescription(),
        this->parseName()
      );
      enumValue.setDirectives(this->parseDirectives());
      enumType->addValue(enumValue);
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
  field.setDirectives(this->parseDirectives());
  field.setName(name);
  field.setInputValueType(type);
  field.setDescription(description);
  return field;
}

void SchemaParser::parseInputObjectTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<InputObjectTypeDefinition> inputType =
    registry.getType<InputObjectTypeDefinition>(this->parseName());
  inputType->setDirectives(this->parseDirectives());
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
  scalar->setDirectives(this->parseDirectives());
  scalar->setDescription(description);
}

cgqlContainer<DirectiveLocation> SchemaParser::parseDirectiveLocations() {
  cgqlContainer<DirectiveLocation> directiveLocations;
  do {
    this->tokenizer.advance();
    directiveLocations.push_back(
      getDirectiveLocation(this->parseName())
    );
  } while(this->checkType(TokenType::PIPE));
  return directiveLocations;
}

void SchemaParser::parseDirectiveTypeDefinition(const TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  this->move(TokenType::AT);
  cgqlSPtr<DirectiveTypeDefinition> directive =
    registry.getType<DirectiveTypeDefinition>(this->parseName());
  directive->setDescription(description);
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      directive->addArgument(this->parseInputValueDefinition(registry));
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }

  assert(
    this->tokenizer.current.value == "on" &&
    "Expected keyword \"on\""
  );
  directive->setDirectiveLocations(this->parseDirectiveLocations());
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
  else if(currentValue == "directive")
    this->parseDirectiveTypeDefinition(registry);
  else {
    assert(false && "Failed to parse type def");
  }
}

void SchemaParser::parse(const TypeRegistry& registry) {
  this->move(TokenType::START_OF_QUERY);
  do {
    this->parseDefinition(registry);
  } while (!this->checkType(TokenType::END_OF_QUERY));
}

cgqlSPtr<Schema> parseSDLSchema(const char *source, const TypeRegistry& registry) {
  SchemaParser parser(source);
  parser.parse(registry);
  cgqlSPtr<Schema> schema = cgqlSMakePtr<Schema>();
  schema->setTypeDefMap(registry.getAllTypes());
  return schema;
}

} /* cgql */ 


