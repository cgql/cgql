#pragma once

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/query/Document.h"

#include "cgql/cgqlDefs.h"

namespace cgql {

struct ExecutionContext {
  cgqlSPtr<Schema> schema;
  ResolverMap resolverMap;
  TypeOfMap typeOfMap;
  cgqlContainer<FragmentDefinition> fragments;
};

FieldTypeDefinition findGraphQLFieldByName(
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const std::string& fieldName
);

void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedField
);

cgqlSPtr<Object> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query,
  const cgqlSPtr<Schema>& schema
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

cgqlSPtr<Object> execute(
  const cgqlSPtr<Schema>& schema,
  const Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
