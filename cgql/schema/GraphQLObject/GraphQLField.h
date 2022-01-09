#ifndef GRAPHQL_FIELD_H
#define GRAPHQL_FIELD_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/type/abstractType.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

struct TypeMetaData {
public:
  inline void setWrappedInnerType(const cgqlSPtr<TypeMetaData>& wrappedInnerType) {
    this->wrappedInnerType = wrappedInnerType;
  }
  inline void setIsList(bool isList) { this->_isList = isList; }
  inline void setIsNonNull(bool isNonNull) { this->_isNonNull = isNonNull; }
  inline const cgqlSPtr<TypeMetaData>& getWrappedInnerType() const {
    return this->wrappedInnerType;
  }
  inline bool isList() const {
    return this->_isList;
  }
  inline bool isNonNull() const {
    return this->_isNonNull;
  }
private:
  cgqlSPtr<TypeMetaData> wrappedInnerType;
  bool _isList;
  bool _isNonNull;
};

class GraphQLArgument : public internal::AbstractTypeDefinition {
public:
  GraphQLArgument(
    const std::string& name,
    const GraphQLScalarTypes& type
  );
  GraphQLArgument() = default;
  ~GraphQLArgument();
  inline void setType(const GraphQLScalarTypes& type) {
    this->type = type;
  }
  inline const GraphQLScalarTypes& getType() const {
    return this->type;
  }
  inline void setTypeMetaData(const TypeMetaData& typeMetaData) {
    this->typeMetaData = typeMetaData;
  }
  inline const TypeMetaData& getTypeMetaData() const {
    return this->typeMetaData;
  }
private:
  std::string name;
  GraphQLScalarTypes type;
  TypeMetaData typeMetaData;
};

class GraphQLField : public internal::AbstractTypeDefinition {
public:
  GraphQLField(
    const std::string& name,
    const GraphQLScalarTypes& type,
    const std::optional<cgql::ResolverFunc>& resolve
  );
  GraphQLField(
    const std::string& name,
    const GraphQLScalarTypes& type
  );
  GraphQLField() = default;
  ~GraphQLField();
  inline const GraphQLScalarTypes& getType() const { return this->type; };
  inline const std::optional<cgql::ResolverFunc>& getResolver() const { return this->resolve; };
  inline void setType(const GraphQLScalarTypes& type) {
    this->type = type;
  }
  inline void addArg(
    const std::string& name,
    const GraphQLArgument& arg
  ) {
    this->args.push_back(arg);
  }
  inline cgqlContainer<GraphQLArgument> getArgs() const {
    return this->args;
  };
  inline void setTypeMetaData(const TypeMetaData& typeMetaData) {
    this->typeMetaData = typeMetaData;
  }
  inline const TypeMetaData& getTypeMetaData() const {
    return this->typeMetaData;
  }
private:
  GraphQLScalarTypes type;
  TypeMetaData typeMetaData;
  std::optional<cgql::ResolverFunc> resolve;
  cgqlContainer<GraphQLArgument> args;
};

} // cgql

#endif
