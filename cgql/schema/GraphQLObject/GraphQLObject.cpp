#include "GraphQLObject.h"

GraphQLObject::GraphQLObject(
  const char* name,
  const vector<GraphQLField>& fields
): name(name), fields(fields) {}

GraphQLObject::~GraphQLObject() {}
