#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

void TypeRegistry::init() {
  auto IntType = cgqlSMakePtr<ScalarTypeDefinition<Int>>(
    "Int"
  );
  this->addType(IntType);
  auto StringType = cgqlSMakePtr<ScalarTypeDefinition<String>>(
    "String"
  );
  this->addType(StringType);
}

cgqlSPtr<TypeDefinition> TypeRegistry::getType(const std::string &typeName) const {
  // static std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeAccessCache;
  // auto const& cachedType = typeAccessCache.find(typeName);
  // if(cachedType != typeAccessCache.end()) return cachedType->second;
  auto const& it = this->types.find(typeName);
  cgqlAssert(it == this->types.end(), "Unable to find type");
  return it->second;
}

} // end of internal
} // end of cgql
