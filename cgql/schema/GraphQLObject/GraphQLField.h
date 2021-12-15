#ifndef GRAPHQL_FIELD_H
#define GRAPHQL_FIELD_H

#include "../../cgqlPch.h"

#include "../GraphQLTypes.h"

namespace cgql {

using std::string;

class GraphQLField {
public:
  GraphQLField(
    const string& name,
    const GraphQLScalarTypes& type,
    const std::optional<cgql::ResolverFunc>& resolve
  );
  GraphQLField(
    const string& name,
    const GraphQLScalarTypes& type
  );
  GraphQLField() = default;
  ~GraphQLField();
  inline const string& getName() const { return this->name; };
  inline const GraphQLScalarTypes& getType() const { return this->type; };
  inline const std::optional<cgql::ResolverFunc>& getResolver() const { return this->resolve; };
private:
  string name;
  GraphQLScalarTypes type;
  std::optional<cgql::ResolverFunc> resolve;
};

} // cgql

#endif
