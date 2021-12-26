#ifndef GRAPHQL_FIELD_H
#define GRAPHQL_FIELD_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/type/abstractType.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

class GraphQLArgument : public internal::AbstractTypeDefinition {
public:
  GraphQLArgument(
    const std::string& name,
    const GraphQLScalarTypes& type
  );
  GraphQLArgument() = default;
  ~GraphQLArgument();
  inline void setType(const GraphQLScalarTypes& type) {
    this->type = type;
  }
  inline const GraphQLScalarTypes& getType() const {
    return this->type;
  }
private:
  std::string name;
  GraphQLScalarTypes type;
};

class GraphQLField : public internal::AbstractTypeDefinition {
public:
  GraphQLField(
    const std::string& name,
    const GraphQLScalarTypes& type,
    const std::optional<cgql::ResolverFunc>& resolve
  );
  GraphQLField(
    const std::string& name,
    const GraphQLScalarTypes& type
  );
  GraphQLField() = default;
  ~GraphQLField();
  inline const GraphQLScalarTypes& getType() const { return this->type; };
  inline const std::optional<cgql::ResolverFunc>& getResolver() const { return this->resolve; };
  inline void setType(const GraphQLScalarTypes& type) {
    this->type = type;
  }
  inline void addArg(
    const std::string& name,
    const GraphQLArgument& arg
  ) {
    this->args.push_back(arg);
  }
  inline const cgqlContainer<GraphQLArgument>& getArgs() const {
    return this->args;
  };
private:
  GraphQLScalarTypes type;
  std::optional<cgql::ResolverFunc> resolve;
  cgqlContainer<GraphQLArgument> args;
};

} // cgql

#endif
