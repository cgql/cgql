#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

#define MAKE_SCALAR(name, DataType) cgqlSMakePtr<ScalarTypeDefinition<DataType>>(name)

void TypeRegistry::init() {
  this->addType(MAKE_SCALAR("Int", Int));
  this->addType(MAKE_SCALAR("String", String));
}

cgqlSPtr<TypeDefinition> TypeRegistry::getType(std::string typeName) const {
  // static std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeAccessCache;
  // auto const& cachedType = typeAccessCache.find(typeName);
  // if(cachedType != typeAccessCache.end()) return cachedType->second;
  return this->types[typeName];
}

} // end of internal
} // end of cgql
