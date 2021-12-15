#ifndef GRAPHQL_TYPES_H
#define GRAPHQL_TYPES_H

#include "../cgqlPch.h"
#include "./GraphQLScalar.h"

namespace cgql {

class GraphQLObject;

namespace GraphQLTypes {
  inline GraphQLTypesBase<Int> GraphQLInt(
    "Int",
    [](Int value) -> Int {
      return value;
    }
  );
  inline GraphQLTypesBase<String> GraphQLString(
    "String",
    [](String value) -> String {
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
  typedef std::unordered_map<string, std::vector<internal::Field>> GroupedField;
}

struct ResultMap;
typedef std::variant<
  GraphQLReturnTypes,
  cgqlSPtr<ResultMap>
> Data;

struct ResultMap {
  std::unordered_map<string, Data> data;
};

using ResolverFunc = function<
  Data()
>;

}


#endif
