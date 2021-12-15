#include "GraphQLField.h"

namespace cgql {

GraphQLField::GraphQLField(
  const string& name,
  const GraphQLScalarTypes& type,
  const std::optional<cgql::ResolverFunc>& resolve
): name(name), type(type), resolve(resolve) {}

GraphQLField::GraphQLField(
  const string& name,
  const GraphQLScalarTypes& type
): name(name), type(type) {}

GraphQLField::~GraphQLField() {}

} // cgql
