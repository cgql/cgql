#ifndef GRAPHQL_TYPES_H
#define GRAPHQL_TYPES_H

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/utilities/assert.h"

namespace cgql {

using GraphQLReturnTypes = std::variant<
  Int,
  String,
  cgqlSPtr<internal::ObjectTypeDefinition>
>;

}

namespace cgql {

namespace internal {
  class Field;
  using GroupedField = std::unordered_map<
    std::string,
    cgqlContainer<cgqlSPtr<Field>>
  >;
}

struct ResultMap;
struct Location {
  uint16_t line;
  uint16_t column;
};

using Data = std::variant<
  GraphQLReturnTypes,
  cgqlSPtr<ResultMap>,
  cgqlContainer<GraphQLReturnTypes>,
  cgqlContainer<cgqlSPtr<ResultMap>>,
  std::monostate
>;

class Error {
public:
  Error() = default;
private:
  std::string msg;
  Location location;
};

struct ResultMap {
  std::unordered_map<std::string, Data> data;
  std::list<Error> errors;
  std::string __typename;
};

using Arg = std::variant<
  Int,
  String
>;

using ArgsMap = std::unordered_map<
  std::string,
  Arg
>;

struct Args {
  ArgsMap argsMap;
  inline const Arg& operator[](const std::string& argKey) const {
    return this->argsMap.at(argKey);
  }
};

template<class T, class... Types>
constexpr const T& fromVariant(
  const std::variant<Types...>& v
) {
  return std::get<T>(v);
}

using ResolverFunc = std::function<
  Data(const Args&)
>;

using ResolverMap = std::unordered_map<std::string, ResolverFunc>;

}


#endif
