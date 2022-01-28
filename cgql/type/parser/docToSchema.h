#ifndef DOC_TO_SCHEMA_H
#define DOC_TO_SCHEMA_H

#include "cgql/base/cgqlPch.h"
#include "cgql/cgql.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/assert.h"

namespace cgql {
namespace internal {

class DocToSchema {
public:
  DocToSchema(TypeRegistry registry)
    : registry(registry) {}
  void completeImplementedInterface(
    cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& interfaces
  );
  void completeObject(
    cgqlSPtr<ObjectTypeDefinition> const& object
  );
  void completeInterface(
    cgqlSPtr<InterfaceTypeDefinition> const& interface
  );
  void completeField(
    FieldTypeDefinition const& field
  );
  void completeArgument(
    ArgumentTypeDefinition const& argument
  );
  void completeType(
    cgqlSPtr<TypeDefinition>& type
  );
private:
  TypeRegistry registry;
};

} // end of internal
} // end of cgql

#endif /* end of include guard: DOC_TO_SCHEMA_H */
