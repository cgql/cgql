#include "GraphQLObject.h"

GraphQLObject::GraphQLObject(
  string name,
  const vector<GraphQLField>& fields
): name(name), fields(fields) {}

GraphQLObject::~GraphQLObject() {}
