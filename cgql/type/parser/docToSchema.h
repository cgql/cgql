#ifndef DOC_TO_SCHEMA_H
#define DOC_TO_SCHEMA_H

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/assert.h"

namespace cgql {
namespace internal {

class DocToSchema {
public:
  DocToSchema() = default;

  void completeObject(
    cgqlSPtr<ObjectTypeDefinition> const& object,
    std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeDefMap
  );
  void completeField(
    FieldTypeDefinition const& field,
    std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeDefMap
  );
  void completeArgument(
    ArgumentTypeDefinition const& argument,
    std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeDefMap
  );
  void completeType(
    cgqlSPtr<TypeDefinition>& type,
    std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeDefMap
  );
};

} // end of internal
} // end of cgql

#endif /* end of include guard: DOC_TO_SCHEMA_H */
