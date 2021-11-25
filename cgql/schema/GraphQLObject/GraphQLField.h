#include "../../utils/pch.h"
#include "../GraphQLScalarTypes.h"
#include <unordered_map>
#include <variant>
#include <functional>

using std::variant;
using std::string;
using std::unordered_map;

class GraphQLObject;

using GraphQLOutputTypes = variant<
  GraphQLScalarType<Int>,
  GraphQLScalarType<String>,
  GraphQLObject*
>;

using GraphQLInputTypes = variant<
  GraphQLScalarType<Int>,
  GraphQLScalarType<String>
>;

using ResolverFunc = function<
  GraphQLOutputTypes(unordered_map<string, GraphQLInputTypes>)
>;

class GraphQLField {
public:
  GraphQLField(
    string name,
    GraphQLOutputTypes type,
    ResolverFunc resolve
  );
  ~GraphQLField();
private:
  string name;
  GraphQLOutputTypes type;
  ResolverFunc resolve;
};
