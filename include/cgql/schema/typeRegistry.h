#pragma once

#include "cgql/schema/typeDefinitions.h"
#include "cgql/cgqlDefs.h"

namespace cgql {

class TypeRegistry {
public:
  void addScalar(cgqlSPtr<ScalarTypeDefinition> type) const;
  cgqlSPtr<TypeDefinition> getType(std::string typeName) const;
  template<typename T>
  cgqlSPtr<T> registerType(std::string typeName) const {
    auto& type = this->types[typeName] = cgqlSMakePtr<T>();
    type->setName(typeName);
    return std::static_pointer_cast<T>(type);
  }
  void init();
  const auto& getAllTypes() const {
    return this->types;
  }
private:
  mutable std::unordered_map<std::string, cgqlSPtr<TypeDefinition>> types;
};

} // end of cgql
