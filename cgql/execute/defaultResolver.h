#ifndef DEFAULT_RESOLVER_H
#define DEFAULT_RESOLVER_H

#include "../schema/GraphQLDefinition.h"
#include "execute.h"

namespace cgql {
namespace internal {

using std::string;

inline Data defaultFieldResolver(
  const ResultMap& source,
  const string& name
) {
  // auto it = source.data.find(name);
  Data result = source.data.at(name);
  return result;
}

} // internal
} // cgql
#endif
