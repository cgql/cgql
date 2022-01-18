#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

TypeRegistry::TypeRegistry() {}
TypeRegistry::~TypeRegistry() {}

template<typename T>
void TypeRegistry::addType(const T& type) {
  this->types.push_back(cgqlSMakePtr<T>(type));
}

cgqlSPtr<TypeDefinition> TypeRegistry::getType(const std::string &typeName) {
  static std::unordered_map<std::string, const cgqlSPtr<TypeDefinition>&> typeAccessCache;
  auto const& cachedType = typeAccessCache.find(typeName);
  if(cachedType != typeAccessCache.end()) return cachedType->second;
  for(auto const& type : this->types) {
    if(type->getName() == typeName) {
      typeAccessCache.try_emplace(typeName, type);
      return type;
    }
  }
  return nullptr;
}

} // end of internal
} // end of cgql
