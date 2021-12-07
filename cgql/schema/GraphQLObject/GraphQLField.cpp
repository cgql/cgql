#include "GraphQLField.h"

GraphQLField::GraphQLField(
  string name,
  GraphQLScalarTypes type,
  ResolverFunc resolve
): name(name), type(type), resolve(resolve) {}

GraphQLField::~GraphQLField() {}
