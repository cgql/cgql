#include "cgql/schema/GraphQLObject/GraphQLField.h"

namespace cgql {

// TypeMetaData
void TypeMetaData::setWrappedInnerType(const cgqlSPtr<TypeMetaData> &wrappedInnerType) {
  this->wrappedInnerType = wrappedInnerType;
}

void TypeMetaData::setIsList(bool isList) { this->_isList = isList; }

void TypeMetaData::setIsNonNull(bool isNonNull) { this->_isNonNull = isNonNull; }

const cgqlSPtr<TypeMetaData>& TypeMetaData::getWrappedInnerType() const {
  return this->wrappedInnerType;
}

bool TypeMetaData::isList() const {
  return this->_isList;
}
bool TypeMetaData::isNonNull() const {
  return this->_isNonNull;
}

GraphQLArgument::GraphQLArgument(
  const std::string& name,
  const GraphQLScalarTypes& type
): type(type) {
  this->setName(name);
}

GraphQLArgument::~GraphQLArgument() {}

void GraphQLArgument::setType(const GraphQLScalarTypes& type) {
  this->type = type;
}
const GraphQLScalarTypes& GraphQLArgument::getType() const {
  return this->type;
}
void GraphQLArgument::setTypeMetaData(const TypeMetaData& typeMetaData) {
  this->typeMetaData = typeMetaData;
}
const TypeMetaData& GraphQLArgument::getTypeMetaData() const {
  return this->typeMetaData;
}

// GraphQLField
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

const GraphQLScalarTypes& GraphQLField::getType() const { return this->type; };

const std::optional<cgql::ResolverFunc>& GraphQLField::getResolver() const { return this->resolve; };

void GraphQLField::setType(const GraphQLScalarTypes& type) {
  this->type = type;
}

void GraphQLField::addArg(
  const std::string& name,
  const GraphQLArgument& arg
) {
  this->args.push_back(arg);
}

const cgqlContainer<GraphQLArgument>& GraphQLField::getArgs() const {
  return this->args;
};

void GraphQLField::setTypeMetaData(const TypeMetaData& typeMetaData) {
  this->typeMetaData = typeMetaData;
}

const TypeMetaData& GraphQLField::getTypeMetaData() const {
  return this->typeMetaData;
}

} // cgql
