#pragma once

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

class TypeRegistry {
public:
  TypeRegistry() = default;
  template<typename T>
  void addScalar(cgqlSPtr<T> type) const {
    this->types.try_emplace(type->getName(), type);
  };
  template<typename T>
  cgqlSPtr<T> getType(std::string typeName) const {
    return defaultConstruct<T>(typeName);
  }
  template<typename T>
  cgqlSPtr<T> getTypePtr(std::string typeName) const {
    return types[typeName];
  }
  template<typename T>
  cgqlSPtr<T> defaultConstruct(std::string typeName) const {
    this->types[typeName] = cgqlSMakePtr<T>();
    this->types[typeName]->setName(typeName);
    return std::static_pointer_cast<T>(this->types[typeName]);
  }
  void init();
  auto getAllTypes() const {
    return this->types;
  }
private:
  mutable std::unordered_map<std::string, cgqlSPtr<TypeDefinition>> types;
};

} // end of internal
} // end of cgql
