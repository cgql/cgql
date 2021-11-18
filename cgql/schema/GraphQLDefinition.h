#include "../utils/pch.h"
#include <unordered_map>
#include <variant>

#include "GraphQLScalarTypes.h"

using std::string;
using std::unordered_map;
using std::variant;

struct GraphQLField;
struct GraphQLObject {
  string name;
  unordered_map<string, GraphQLField> fields;
};
struct GraphQLField {
  variant<
    GraphQLScalarType<Int>,
    GraphQLScalarType<String>,
    GraphQLObject
  > type;
};
