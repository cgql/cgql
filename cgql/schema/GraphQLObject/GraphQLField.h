#include "../../cgqlPch.h"

#include "../GraphQLTypes.h"

using std::variant;
using std::string;
using std::shared_ptr;
using std::optional;

class GraphQLObject;

using GraphQLScalarTypes = variant<
  GraphQLTypesBase<Int>,
  GraphQLTypesBase<String>,
  GraphQLTypesBase<shared_ptr<GraphQLObject>>
>;

using GraphQLReturnTypes = variant<
  Int,
  String,
  shared_ptr<GraphQLObject>
>;

using ResolverFunc = function<
  GraphQLReturnTypes()
>;

class GraphQLField {
public:
  GraphQLField(
    string name,
    GraphQLScalarTypes type,
    optional<ResolverFunc> resolve
  );
  ~GraphQLField();
  inline const string& getName() const { return this->name; };
  inline const GraphQLScalarTypes& getType() const { return this->type; };
  inline const optional<ResolverFunc>& getResolver() const { return this->resolve; };
private:
  string name;
  GraphQLScalarTypes type;
  optional<ResolverFunc> resolve;
};
