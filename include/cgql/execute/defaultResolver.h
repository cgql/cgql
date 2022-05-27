#pragma once

#include "cgql/schema/GraphQLTypes.h"

namespace cgql {
namespace internal {

inline Data defaultFieldResolver(
  const cgqlSPtr<Object>& source,
  const std::string& name
) {
  auto const& it = source->fields.find(name);
  if(it == source->fields.end()) {
    return std::monostate{};
  }
  return it->second;
}

} // internal
} // cgql
