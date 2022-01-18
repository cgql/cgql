#ifndef TYPE_REGISTRY_H
#define TYPE_REGISTRY_H

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

class TypeRegistry {
public:
  TypeRegistry();
  ~TypeRegistry();

  template<typename T>
  void addType(const T& type);
  cgqlSPtr<TypeDefinition> getType(const std::string& typeName);
private:
  cgqlContainer<cgqlSPtr<TypeDefinition>> types;
};

} // end of internal
} // end of cgql

#endif /* end of include guard: TYPE_REGISTRY_H */
