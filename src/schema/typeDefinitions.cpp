#include "cgql/schema/typeDefinitions.h"

namespace cgql {

void AbstractSchemaTypeDefinition::setName(std::string name) {
  this->name = name;
}
const std::string& AbstractSchemaTypeDefinition::getName() const {
  return this->name;
}
void AbstractSchemaTypeDefinition::setDescription(std::string description) {
  this->description = description;
}
const std::string& AbstractSchemaTypeDefinition::getDescription() const {
  return this->description;
}

Directive::Directive(std::string name) {
  this->setName(name);
}
void Directive::addArgument(Directive::DirectiveArgument argument) {
  this->args.push_back(argument);
}
const cgqlContainer<Directive::DirectiveArgument>& Directive::getArguments() const {
  return this->args;
}

void TypeDefinitionWithDirectives::setDirectives(cgqlContainer<Directive> directives) {
  this->directives = directives;
}
const cgqlContainer<Directive>& TypeDefinitionWithDirectives::getDirectives() const {
  return this->directives;
}

TypeDefinition::~TypeDefinition() = default;

ScalarTypeDefinition::ScalarTypeDefinition() = default;
ScalarTypeDefinition::ScalarTypeDefinition(
  std::string name,
  Serializer serializer
) : serializer(serializer) {
  this->setName(name);
}

ListTypeDefinition::ListTypeDefinition(cgqlSPtr<TypeDefinition> innerType)
  : innerType(innerType) {
}
const cgqlSPtr<TypeDefinition>& ListTypeDefinition::getInnerType() const {
  return this->innerType;
};

NonNullTypeDefinition::NonNullTypeDefinition(cgqlSPtr<TypeDefinition> innerType)
  : innerType(innerType) {
}
const cgqlSPtr<TypeDefinition>& NonNullTypeDefinition::getInnerType() const {
  return this->innerType;
};

DefaultWrapTypeDefinition::DefaultWrapTypeDefinition(cgqlSPtr<TypeDefinition> innerType) 
  : innerType(innerType) {
}
cgqlSPtr<TypeDefinition> DefaultWrapTypeDefinition::getInnerType() const {
  return innerType.lock();
};

void InputValueDefinition::setDefaultValue(GraphQLInputTypes value) {
  this->defaultValue = value;
}
const GraphQLInputTypes& InputValueDefinition::getDefaultValue() const {
  return this->defaultValue;
}
void InputValueDefinition::setInputValueType(cgqlSPtr<TypeDefinition> type) {
  this->type = type;
}
const cgqlSPtr<TypeDefinition>& InputValueDefinition::getInputValueType() const {
  return this->type;
}

void FieldTypeDefinition::setFieldType(cgqlSPtr<TypeDefinition> type) {
  this->type = type;
}
const cgqlSPtr<TypeDefinition>& FieldTypeDefinition::getFieldType() const {
  return this->type;
}
void FieldTypeDefinition::addArg(InputValueDefinition arg) {
  this->argDefs.push_back(arg);
}
const cgqlContainer<InputValueDefinition>& FieldTypeDefinition::getArgs() const {
  return this->argDefs;
}

void InterfaceTypeDefinition::addField(FieldTypeDefinition field) {
  this->fields.push_back(field);
}
const cgqlContainer<FieldTypeDefinition>& InterfaceTypeDefinition::getFields() const {
  return this->fields;
}
void InterfaceTypeDefinition::addImplementedInterface(std::string interface) {
  this->implements.push_back(interface);
}
const cgqlContainer<std::string>& InterfaceTypeDefinition::getImplementedInterfaces() const {
  return this->implements;
}

void ObjectTypeDefinition::addField(FieldTypeDefinition field) {
  this->fieldDefs.push_back(field);
}
const cgqlContainer<FieldTypeDefinition>& ObjectTypeDefinition::getFields() const {
  return this->fieldDefs;
}
void ObjectTypeDefinition::addImplementedInterface(std::string interface) {
  this->implements.push_back(interface);
}
const cgqlContainer<std::string>& ObjectTypeDefinition::getImplementedInterfaces() const {
  return this->implements;
}

void UnionTypeDefinition::addMember(cgqlSPtr<TypeDefinition> member) {
  this->members.push_back(member);
}
const cgqlContainer<cgqlSPtr<TypeDefinition>>& UnionTypeDefinition::getMembers() const {
  return members;
}


EnumValueDefinition::EnumValueDefinition(
  std::string description,
  std::string name
) {
  this->setDescription(description);
  this->setName(name);
}


void EnumTypeDefinition::addValue(EnumValueDefinition value) {
  values.push_back(value);
}
const cgqlContainer<EnumValueDefinition>& EnumTypeDefinition::getValues() const {
  return this->values;
}
String EnumTypeDefinition::serialize(String outputValue) {
  cgqlContainer<EnumValueDefinition>::const_iterator it =
    std::find_if(
      values.begin(),
      values.end(),
      [&](const EnumValueDefinition& value) {
        return outputValue == value.getName();
      }
    );
  if(it != values.end())
    return outputValue;
  throw outputValue;
}

void InputObjectTypeDefinition::addField(InputValueDefinition field) {
  fields.push_back(field);
}
const cgqlContainer<InputValueDefinition>& InputObjectTypeDefinition::getFields() const {
  return this->fields;
}


void DirectiveTypeDefinition::addArgument(InputValueDefinition argument) {
  arguments.push_back(argument);
}
const cgqlContainer<InputValueDefinition>& DirectiveTypeDefinition::getArguments() const {
  return this->arguments;
}
void DirectiveTypeDefinition::setDirectiveLocations(cgqlContainer<DirectiveLocation> locations) {
  this->locations = locations;
}
const cgqlContainer<DirectiveLocation>& DirectiveTypeDefinition::getDirectiveLocations() const {
  return this->locations;
}

void Schema::setQuery(cgqlSPtr<ObjectTypeDefinition> query) {
  this->query = query;
}
const cgqlSPtr<ObjectTypeDefinition>& Schema::getQuery() const {
  return this->query;
}
void Schema::setTypeDefMap(
  const std::unordered_map<std::string, cgqlSPtr<TypeDefinition>>& typeDefMap
) {
  for(auto const& [key, def] : typeDefMap) {
    assert(
      def->getDefinitionType() != DefinitionType::TYPE_DEFINITION &&
      "type is empty"
    );
    auto addImplementedInterface = [this](
      const cgqlContainer<std::string>& implements,
      cgqlSPtr<TypeDefinition> def
    ) {
      for(auto interface : implements) {
        this->implementedInterfaces[interface]
          .push_back(def);
      }
    };
    switch(def->getDefinitionType()) {
      case DefinitionType::OBJECT: {
        cgqlSPtr<ObjectTypeDefinition> object =
          std::static_pointer_cast<ObjectTypeDefinition>(def);
        if(object->getName() == "Query") {
          this->setQuery(object);
        }
        addImplementedInterface(object->getImplementedInterfaces(), def);
        break;
      }
      case DefinitionType::INTERFACE: {
        cgqlSPtr<InterfaceTypeDefinition> interface =
          std::static_pointer_cast<InterfaceTypeDefinition>(def);
        addImplementedInterface(interface->getImplementedInterfaces(), def);
        break;
      }
      default: continue;
    }
  }
}

}
