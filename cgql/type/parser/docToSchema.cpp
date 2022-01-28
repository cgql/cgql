#include "cgql/type/parser/docToSchema.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

void DocToSchema::completeImplementedInterface(
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& interfaces
) {
  for(cgqlSPtr<InterfaceTypeDefinition>& interface : interfaces) {
    cgqlSPtr<TypeDefinition> interfaceAsTypeDef =
      std::static_pointer_cast<TypeDefinition>(interface);
    this->completeType(interfaceAsTypeDef);
  }
}

void DocToSchema::completeObject(
  cgqlSPtr<ObjectTypeDefinition> const &object
) {
  for(FieldTypeDefinition& field : object->getFields()) {
    this->completeField(field);
  }
  this->completeImplementedInterface(object->getImplementedInterfaces());
}

void DocToSchema::completeInterface(
  cgqlSPtr<InterfaceTypeDefinition> const& interface
) {
  for(FieldTypeDefinition& field : interface->getFields()) {
    this->completeField(field);
  }
  this->completeImplementedInterface(interface->getImplementedInterfaces());
}

void DocToSchema::completeField(
  FieldTypeDefinition const& field
) {
  this->completeType(field.getType());
  for(ArgumentTypeDefinition& argument : field.getArgs()) {
    this->completeArgument(argument);
  }
}

void DocToSchema::completeArgument(
  ArgumentTypeDefinition const& argument
) {
  this->completeType(argument.getType());
}

void DocToSchema::completeType(
  cgqlSPtr<TypeDefinition>& type
) {
  if(type->getType() == DefinitionType::LIST_TYPE) {
    ListTypeDefinition<TypeDefinition>& list =
      static_cast<ListTypeDefinition<TypeDefinition>&>(*type);
    this->completeType(list.getInnerType());
    return;
  }
  if(type->getType() == DefinitionType::NON_NULL_TYPE) {
    NonNullTypeDefinition<TypeDefinition>& nonNull =
      static_cast<NonNullTypeDefinition<TypeDefinition>&>(*type);
    this->completeType(nonNull.getInnerType());
    return;
  }
  type = this->registry.getType(type->getName());
}

} // end of internal
} // end of cgql
