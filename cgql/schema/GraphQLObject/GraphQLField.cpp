#include "GraphQLField.h"

GraphQLField::GraphQLField(
  string name,
  GraphQLOutputTypes type,
  ResolverFunc resolve
): name(name), type(type), resolve(resolve) {}

GraphQLField::~GraphQLField() {}
