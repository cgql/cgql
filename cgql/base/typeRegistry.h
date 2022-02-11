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
  void addType(const cgqlSPtr<T>& type) const {
    this->types.insert_or_assign(type->getName(), type);
  };
  template<typename T>
  cgqlSPtr<T> getType(std::string typeName) const {
    auto type = this->types[typeName];
    if(type) return std::static_pointer_cast<T>(type);
    type = cgqlSMakePtr<T>();
    type->setName(typeName);
    return std::static_pointer_cast<T>(type);
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
