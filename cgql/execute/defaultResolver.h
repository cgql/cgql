#pragma once

#include "cgql/schema/GraphQLTypes.h"

namespace cgql {
namespace internal {

inline const std::optional<Data> defaultFieldResolver(
  const cgqlSPtr<Object>& source,
  const std::string& name
) {
  auto const& it = source->fields.find(name);
  if(it != source->fields.end()) {
    return it->second;
  }
  return {};
}

} // internal
} // cgql
