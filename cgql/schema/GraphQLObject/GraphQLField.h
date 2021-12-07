#include "../GraphQLTypes.h"
#include <variant>
#include <string>
#include <functional>

using std::variant;
using std::string;

class GraphQLObject;

using GraphQLScalarTypes = variant<
  GraphQLTypesBase<Int>,
  GraphQLTypesBase<String>,
  GraphQLTypesBase<GraphQLObject*>
>;

using GraphQLReturnTypes = variant<
  Int,
  String,
  GraphQLObject*
>;

using ResolverFunc = function<
  GraphQLReturnTypes()
>;

class GraphQLField {
public:
  GraphQLField(
    string name,
    GraphQLScalarTypes type,
    ResolverFunc resolve
  );
  ~GraphQLField();
  inline string getName() const { return this->name; };
  inline GraphQLScalarTypes getType() const { return this->type; };
  inline ResolverFunc getResolver() const { return this->resolve; };
private:
  string name;
  GraphQLScalarTypes type;
  ResolverFunc resolve;
};
