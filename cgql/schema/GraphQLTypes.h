#ifndef GRAPHQL_TYPES_H
#define GRAPHQL_TYPES_H

#include "../cgqlPch.h"
#include "./GraphQLScalar.h"

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
  inline GraphQLTypesBase<std::shared_ptr<GraphQLObject>> GraphQLObjectType(
    "Object",
    [](std::shared_ptr<GraphQLObject> value) -> std::shared_ptr<GraphQLObject> {
      return value;
    }
  );
}

class GraphQLObject;

using GraphQLScalarTypes = std::variant<
  GraphQLTypesBase<Int>,
  GraphQLTypesBase<String>,
  GraphQLTypesBase<std::shared_ptr<GraphQLObject>>
>;

using GraphQLReturnTypes = std::variant<
  Int,
  String,
  std::shared_ptr<GraphQLObject>
>;

namespace cgql {

namespace internal {
  class Field;
}

typedef std::unordered_map<string, std::vector<internal::Field>> GroupedField;

struct ResultMap;
typedef std::variant<
  GraphQLReturnTypes,
  std::shared_ptr<ResultMap>
> Data;

struct ResultMap {
  std::unordered_map<string, Data> data;
};

using ResolverFunc = function<
  Data()
>;

}


#endif
