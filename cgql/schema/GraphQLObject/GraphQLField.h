#ifndef GRAPHQL_FIELD_H
#define GRAPHQL_FIELD_H

#include "../../cgqlPch.h"

#include "../GraphQLTypes.h"
#include "../../type/abstractType.h"

namespace cgql {

using std::string;

class GraphQLField : public internal::AbstractTypeDefinition {
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
  inline const GraphQLScalarTypes& getType() const { return this->type; };
  inline const std::optional<cgql::ResolverFunc>& getResolver() const { return this->resolve; };
  inline void setType(const GraphQLScalarTypes& type) {
    this->type = type;
  }
private:
  GraphQLScalarTypes type;
  std::optional<cgql::ResolverFunc> resolve;
};

} // cgql

#endif
