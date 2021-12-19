#include "GraphQLObject.h"

namespace cgql {

GraphQLObject::GraphQLObject(
  const char* name,
  const vector<GraphQLField>& fields
): fields(fields) {
  this->setName(name);
}

GraphQLObject::~GraphQLObject() {}

} // cgql
