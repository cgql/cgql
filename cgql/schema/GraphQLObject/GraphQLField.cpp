#include "cgql/schema/GraphQLObject/GraphQLField.h"

namespace cgql {

GraphQLArgument::GraphQLArgument(
  const std::string& name,
  const GraphQLScalarTypes& type
): type(type) {
  this->setName(name);
}

GraphQLArgument::~GraphQLArgument() {}

GraphQLField::GraphQLField(
  const std::string& name,
  const GraphQLScalarTypes& type,
  const std::optional<cgql::ResolverFunc>& resolve
): type(type), resolve(resolve) {
  this->setName(name);
}

GraphQLField::GraphQLField(
  const std::string& name,
  const GraphQLScalarTypes& type
): type(type) {
  this->setName(name);
}

GraphQLField::~GraphQLField() {}

} // cgql
