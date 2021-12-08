#include "./GraphQLScalar.h"

using std::shared_ptr;

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
  inline GraphQLTypesBase<shared_ptr<GraphQLObject>> GraphQLObjectType(
    "Object",
    [](shared_ptr<GraphQLObject> value) -> shared_ptr<GraphQLObject> {
      return value;
    }
  );
}
