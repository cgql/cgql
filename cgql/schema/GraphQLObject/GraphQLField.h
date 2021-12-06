#include "../GraphQLScalarTypes.h"
#include <variant>
#include <string>
#include <functional>

using std::variant;
using std::string;

class GraphQLObject;

using GraphQLType = variant<
  GraphQLScalarType<Int>,
  GraphQLScalarType<String>,
  GraphQLObject*
>;

using GraphQLOutputTypes = variant<
  Int,
  String,
  GraphQLObject*
>;

using GraphQLInputTypes = variant<
  GraphQLScalarType<Int>,
  GraphQLScalarType<String>
>;

using ResolverFunc = function<
  GraphQLOutputTypes()
>;

class GraphQLField {
public:
  GraphQLField(
    string name,
    GraphQLType type,
    ResolverFunc resolve
  );
  ~GraphQLField();
  inline string getName() const { return this->name; };
  inline GraphQLType getType() const { return this->type; };
  inline ResolverFunc getResolver() const { return this->resolve; };
private:
  string name;
  GraphQLType type;
  ResolverFunc resolve;
};
