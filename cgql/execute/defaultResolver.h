#ifndef DEFAULT_RESOLVER_H
#define DEFAULT_RESOLVER_H

#include "cgql/schema/GraphQLTypes.h"

namespace cgql {
namespace internal {

inline const std::optional<Data> defaultFieldResolver(
  const ResultMap& source,
  const std::string& name
) {
  auto const& it = source.data.find(name);
  if(it != source.data.end()) {
    return it->second;
  }
  // cgqlAssert(it == source.data.end(), "Cannot find value for given key in result map");
  return {};
}

} // internal
} // cgql
#endif
