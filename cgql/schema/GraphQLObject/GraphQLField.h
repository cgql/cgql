#include "../../utils/pch.h"
#include "../GraphQLScalarTypes.h"
#include <variant>
 
using std::variant;

class GraphQLObject;

using GraphQLOutputTypes = variant<
  GraphQLScalarType<Int>,
  GraphQLScalarType<String>,
  GraphQLObject*
>;

class GraphQLField {
public:
  GraphQLField(
    string name,
    GraphQLOutputTypes type
  );
  ~GraphQLField();
private:
  string name;
  GraphQLOutputTypes type;
};
