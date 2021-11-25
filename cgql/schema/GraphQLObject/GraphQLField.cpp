#include "GraphQLField.h"

GraphQLField::GraphQLField(
  string name,
  GraphQLOutputTypes type
): name(name), type(type) {}

GraphQLField::~GraphQLField() {}
