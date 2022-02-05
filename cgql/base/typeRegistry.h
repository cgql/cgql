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
  cgqlSPtr<TypeDefinition> getType(std::string typeName) const;
  void init();
  auto getAllTypes() const {
    return this->types;
  }
  auto getOrDefaultConstructType(std::string typeName) const {
    return this->types[typeName];
  }
private:
  mutable std::unordered_map<std::string, cgqlSPtr<TypeDefinition>> types;
};

} // end of internal
} // end of cgql

#endif /* end of include guard: TYPE_REGISTRY_H */
