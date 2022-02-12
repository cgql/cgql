#ifndef TYPE_REGISTRY_H
#define TYPE_REGISTRY_H

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

class TypeRegistry {
public:
  TypeRegistry() = default;
  template<typename T>
  void addType(cgqlSPtr<T> type) const {
    this->types.insert_or_assign(type->getName(), type);
  };
  template<typename T>
  cgqlSPtr<T> getType(std::string typeName) const {
    cgqlSPtr<TypeDefinition> type = this->types[typeName];
    if(type) return std::static_pointer_cast<T>(type);
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

#endif /* end of include guard: TYPE_REGISTRY_H */
