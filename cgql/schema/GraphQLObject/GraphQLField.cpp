#include "GraphQLField.h"

GraphQLField::GraphQLField(
  string name,
  GraphQLType type,
  ResolverFunc resolve
): name(name), type(type), resolve(resolve) {}

GraphQLField::~GraphQLField() {}
