#ifndef GRAPHQL_FIELD_H
#define GRAPHQL_FIELD_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/type/abstractType.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

struct TypeMetaData {
public:
  void setWrappedInnerType(const cgqlSPtr<TypeMetaData>& wrappedInnerType);
  void setIsList(bool isList);
  void setIsNonNull(bool isNonNull);
  const cgqlSPtr<TypeMetaData>& getWrappedInnerType() const;
  bool isList() const;
  bool isNonNull() const;
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
  void setType(const GraphQLScalarTypes& type);
  const GraphQLScalarTypes& getType() const;
  void setTypeMetaData(const TypeMetaData& typeMetaData);
  const TypeMetaData& getTypeMetaData() const;
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
  const GraphQLScalarTypes& getType() const;
  const std::optional<cgql::ResolverFunc>& getResolver() const;
  void setType(const GraphQLScalarTypes& type);
  void addArg(
    const std::string& name,
    const GraphQLArgument& arg
  );
  const cgqlContainer<GraphQLArgument>& getArgs() const;
  void setTypeMetaData(const TypeMetaData& typeMetaData);
  const TypeMetaData& getTypeMetaData() const;
private:
  GraphQLScalarTypes type;
  TypeMetaData typeMetaData;
  std::optional<cgql::ResolverFunc> resolve;
  cgqlContainer<GraphQLArgument> args;
};

} // cgql

#endif
