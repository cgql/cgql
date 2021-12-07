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
  inline GraphQLTypesBase<GraphQLObject*> GraphQLObjectType(
    "Object",
    [](GraphQLObject* value) -> GraphQLObject* {
      return value;
    }
  );
}
