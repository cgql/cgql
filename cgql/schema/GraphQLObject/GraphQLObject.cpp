#include "GraphQLObject.h"

namespace cgql {

GraphQLObject::GraphQLObject(
  const char* name,
  const cgqlContainer<GraphQLField>& fields
): fields(fields) {
  this->setName(name);
}

GraphQLObject::~GraphQLObject() {}

} // cgql
