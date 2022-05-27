#pragma once

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/cgqlDefs.h"

namespace cgql {

class TypeRegistry {
public:
  TypeRegistry() = default;
  void addScalar(cgqlSPtr<ScalarTypeDefinition> type) const {
    this->types.try_emplace(type->getName(), type);
  };
  cgqlSPtr<TypeDefinition> getTypeRef(std::string typeName) const {
    return types[typeName];
  }
  template<typename T>
  cgqlSPtr<T> getType(std::string typeName) const {
    this->types[typeName] = cgqlSMakePtr<T>();
    this->types[typeName]->setName(typeName);
    return std::static_pointer_cast<T>(this->types[typeName]);
  }
  void init();
  const std::unordered_map<std::string, cgqlSPtr<TypeDefinition>>& getAllTypes() const {
    return this->types;
  }
private:
  mutable std::unordered_map<std::string, cgqlSPtr<TypeDefinition>> types;
};

} // end of cgql
