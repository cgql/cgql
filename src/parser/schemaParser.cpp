#include "cgql/parser/schemaParser.h"
#include "cgql/parser/directiveLocations.h"

namespace cgql {

std::string SchemaParser::parseDescription() {
  if(this->tokenizer.current.type != TokenType::BLOCK_STRING && this->tokenizer.current.type != TokenType::STRING) {
    return std::string();
  }
  std::string description = this->tokenizer.current.value;
  this->tokenizer.advance();
  return description;
}

cgqlSPtr<TypeDefinition> SchemaParser::parseType(TypeRegistry& registry) {
  cgqlSPtr<TypeDefinition> type;
  if(!this->checkType(TokenType::SQUARE_BRACES_L)) {
    type =
      cgqlSMakePtr<DefaultWrapTypeDefinition>(
        registry.getType(this->parseName())
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
  std::string name = this->parseName();
  this->move(TokenType::COLON);
  return Directive::DirectiveArgument {
    .name = name,
    .value = this->parseValueLiteral()
  };
}

cgqlContainer<Directive> SchemaParser::parseDirectives() {
  cgqlContainer<Directive> directives;
  while(this->checkType(TokenType::AT)) {
    this->tokenizer.advance();
    Directive& directive = directives.emplace_back();
    directive.setName(this->parseName());
    if(this->checkType(TokenType::BRACES_L)) {
      this->tokenizer.advance();
      do {
        directive.addArgument(this->parseDirectiveArgument());
      } while(!this->checkType(TokenType::BRACES_R));
      this->tokenizer.advance();
    }
  }
  return directives;
}

FieldTypeDefinition SchemaParser::parseFieldTypeDefinition(TypeRegistry& registry) {
  FieldTypeDefinition field;
  field.setDescription(this->parseDescription());
  field.setName(this->parseName());
  if(this->checkType(TokenType::BRACES_L)) {
    this->tokenizer.advance();
    do {
      field.addArg(this->parseInputValueDefinition(registry));
    } while(!this->checkType(TokenType::BRACES_R));
    this->tokenizer.advance();
  }
  this->move(TokenType::COLON);
  field.setFieldType(this->parseType(registry));
  field.setDirectives(this->parseDirectives());
  return field;
}

void SchemaParser::parseObjectTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<ObjectTypeDefinition> obj =
    registry.registerType<ObjectTypeDefinition>(this->parseName());
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
    this->tokenizer.advance();
  }
}

void SchemaParser::parseInterfaceTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<InterfaceTypeDefinition> interface =
    registry.registerType<InterfaceTypeDefinition>(this->parseName());
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
    this->tokenizer.advance();
  }
}

void SchemaParser::parseUnionTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<UnionTypeDefinition> unionType =
    registry.registerType<UnionTypeDefinition>(this->parseName());
  unionType->setDirectives(this->parseDirectives());
  unionType->setDescription(description);
  if(this->checkType(TokenType::EQUAL)) {
    do {
      this->tokenizer.advance();
      unionType->addMember(
        registry.getType(this->parseName())
      );
    } while(this->checkType(TokenType::PIPE));
  }
}

void SchemaParser::parseEnumTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<EnumTypeDefinition> enumType =
    registry.registerType<EnumTypeDefinition>(this->parseName());
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
    this->tokenizer.advance();
  }
}

InputValueDefinition SchemaParser::parseInputValueDefinition(TypeRegistry& registry) {
  InputValueDefinition field;
  field.setDescription(this->parseDescription());
  field.setName(this->parseName());
  this->move(TokenType::COLON);
  field.setInputValueType(this->parseType(registry));
  if(this->checkType(TokenType::EQUAL)) {
    this->tokenizer.advance();
    field.setDefaultValue(this->parseValueLiteral());
  }
  field.setDirectives(this->parseDirectives());
  return field;
}

void SchemaParser::parseInputObjectTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<InputObjectTypeDefinition> inputType =
    registry.registerType<InputObjectTypeDefinition>(this->parseName());
  inputType->setDirectives(this->parseDirectives());
  inputType->setDescription(description);
  if(this->checkType(TokenType::CURLY_BRACES_L)) {
    this->tokenizer.advance();
    do {
      inputType->addField(
        this->parseInputValueDefinition(registry)
      );
    } while(!this->checkType(TokenType::CURLY_BRACES_R));
    this->tokenizer.advance();
  }
}

void SchemaParser::parseScalarTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  cgqlSPtr<ScalarTypeDefinition> scalar =
    registry.registerType<ScalarTypeDefinition>(this->parseName());
  scalar->setDirectives(this->parseDirectives());
  scalar->setDescription(description);
}

cgqlContainer<DirectiveLocation> SchemaParser::parseDirectiveLocations() {
  cgqlContainer<DirectiveLocation> directiveLocations;
  do {
    this->tokenizer.advance();
    directiveLocations.emplace_back(
      getDirectiveLocation(this->parseName())
    );
  } while(this->checkType(TokenType::PIPE));
  return directiveLocations;
}

void SchemaParser::parseDirectiveTypeDefinition(TypeRegistry& registry) {
  std::string description = this->parseDescription();
  this->tokenizer.advance();
  this->move(TokenType::AT);
  cgqlSPtr<DirectiveTypeDefinition> directive =
    registry.registerType<DirectiveTypeDefinition>(this->parseName());
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

void SchemaParser::parseDefinition(TypeRegistry& registry) {
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
  else
    assert(false && "Failed to parse type def");
}

void SchemaParser::parse(TypeRegistry& registry) {
  this->move(TokenType::START_OF_QUERY);
  do {
    this->parseDefinition(registry);
  } while (!this->checkType(TokenType::END_OF_QUERY));
}

} /* cgql */ 


