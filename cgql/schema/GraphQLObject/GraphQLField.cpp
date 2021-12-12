#include "GraphQLField.h"

GraphQLField::GraphQLField(
  string name,
  GraphQLScalarTypes type,
  std::optional<cgql::ResolverFunc> resolve
): name(name), type(type), resolve(resolve) {}

GraphQLField::~GraphQLField() {}
