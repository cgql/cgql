#include "cgql/type/parser/docToSchema.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

void DocToSchema::completeImplementedInterface(
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& interfaces,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  for(cgqlSPtr<InterfaceTypeDefinition>& interface : interfaces) {
    cgqlSPtr<TypeDefinition> interfaceAsTypeDef =
      std::static_pointer_cast<TypeDefinition>(interface);
    this->completeType(
      interfaceAsTypeDef,
      typeDefMap
    );
  }
}

void DocToSchema::completeObject(
  cgqlSPtr<ObjectTypeDefinition> const &object,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  for(FieldTypeDefinition& field : object->getFields()) {
    this->completeField(field, typeDefMap);
  }
  this->completeImplementedInterface(object->getImplementedInterfaces(), typeDefMap);
}

void DocToSchema::completeInterface(
  cgqlSPtr<InterfaceTypeDefinition> const& interface,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  for(FieldTypeDefinition& field : interface->getFields()) {
    this->completeField(field, typeDefMap);
  }
  this->completeImplementedInterface(interface->getImplementedInterfaces(), typeDefMap);
}

void DocToSchema::completeField(
  FieldTypeDefinition const& field,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  this->completeType(field.getType(), typeDefMap);
  for(ArgumentTypeDefinition& argument : field.getArgs()) {
    this->completeArgument(argument, typeDefMap);
  }
}

void DocToSchema::completeArgument(
  ArgumentTypeDefinition const& argument,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  this->completeType(argument.getType(), typeDefMap);
}

void DocToSchema::completeType(
  cgqlSPtr<TypeDefinition>& type,
  const std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&>& typeDefMap
) {
  if(type->getType() == DefinitionType::LIST_TYPE) {
    ListTypeDefinition<TypeDefinition>& list =
      static_cast<ListTypeDefinition<TypeDefinition>&>(*type);
    this->completeType(list.getInnerType(), typeDefMap);
    return;
  }
  if(type->getType() == DefinitionType::NON_NULL_TYPE) {
    NonNullTypeDefinition<TypeDefinition>& nonNull =
      static_cast<NonNullTypeDefinition<TypeDefinition>&>(*type);
    this->completeType(nonNull.getInnerType(), typeDefMap);
    return;
  }
  switch(type->getType()) {
    case INT_TYPE: type = BuiltinTypes::IntType; break;
    case STRING_TYPE: type = BuiltinTypes::StringType; break;
    default:
      auto const& it = typeDefMap.find(type->getName());
      cgqlAssert(it == typeDefMap.end(), "Unable to find required type in schema");
      type = it->second;
  }
}

} // end of internal
} // end of cgql
