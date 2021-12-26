#ifndef GRAPHQL_TYPES_H
#define GRAPHQL_TYPES_H

#include "../cgqlPch.h"
#include "./GraphQLScalar.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

class GraphQLObject;

namespace GraphQLTypes {
  inline GraphQLTypesBase<Int> GraphQLInt(
    "Int",
    [](const Int& value) -> Int {
      return value;
    }
  );
  inline GraphQLTypesBase<String> GraphQLString(
    "String",
    [](const String& value) -> String {
      return value;
    }
  );
}

using GraphQLScalarTypes = std::variant<
  GraphQLTypesBase<Int>,
  GraphQLTypesBase<String>,
  cgqlSPtr<GraphQLObject>
>;

using GraphQLReturnTypes = std::variant<
  Int,
  String,
  cgqlSPtr<GraphQLObject>
>;

}

namespace cgql {

namespace internal {
  class Field;
  typedef std::unordered_map<string, cgqlContainer<internal::Field>> GroupedField;
}

struct ResultMap;
typedef std::variant<
  GraphQLReturnTypes,
  cgqlSPtr<ResultMap>
> Data;

struct ResultMap {
  std::unordered_map<string, Data> data;
};

typedef std::variant<
  Int,
  String
> Arg;
typedef std::unordered_map<
  std::string,
  Arg
> ArgsMap;
struct Args {
  ArgsMap argsMap;
  inline Arg operator[](const std::string& argKey) {
    return this->argsMap.at(argKey);
  }
};

template<class T, class... Types>
constexpr const T& fromVariant(
  const std::variant<Types...>& v
) {
  return std::get<T>(v);
}

using ResolverFunc = function<
  Data(Args)
>;

typedef std::unordered_map<string, ResolverFunc> ResolverMap;

}


#endif
