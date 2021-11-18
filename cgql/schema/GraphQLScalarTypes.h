#include "GraphQLScalar.h"

namespace GraphQLTypes {
  GraphQLScalarType<Int> GraphQLInt(
    "Int",
    [](Int value) -> Int {
      return value;
    }
  );
  GraphQLScalarType<String> GraphQLString(
    "String",
    [](String value) -> String {
      return value;
    }
  );
}