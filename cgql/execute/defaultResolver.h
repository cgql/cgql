#ifndef DEFAULT_RESOLVER_H
#define DEFAULT_RESOLVER_H

#include "cgql/schema/GraphQLTypes.h"

namespace cgql {
namespace internal {

inline Data defaultFieldResolver(
  const ResultMap& source,
  const std::string& name
) {
  // auto it = source.data.find(name);
  Data result = source.data.at(name);
  return result;
}

} // internal
} // cgql
#endif
