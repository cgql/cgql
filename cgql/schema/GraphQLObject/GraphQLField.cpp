#include "GraphQLField.h"

namespace cgql {

GraphQLField::GraphQLField(
  const string& name,
  const GraphQLScalarTypes& type,
  const std::optional<cgql::ResolverFunc>& resolve
): type(type), resolve(resolve) {
  this->setName(name);
}

GraphQLField::GraphQLField(
  const string& name,
  const GraphQLScalarTypes& type
): type(type) {
  this->setName(name);
}

GraphQLField::~GraphQLField() {}

} // cgql
