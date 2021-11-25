#include "./GraphQLScalar.h"

namespace GraphQLTypes {
  inline GraphQLScalarType<Int> GraphQLInt(
    "Int",
    [](Int value) -> Int {
      return value;
    }
  );
  inline GraphQLScalarType<String> GraphQLString(
    "String",
    [](String value) -> String {
      return value;
    }
  );
}
