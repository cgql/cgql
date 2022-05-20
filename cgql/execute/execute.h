#pragma once

#include "cgql/base/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/type/Document.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

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
  GroupedField& groupedFields
);

void mergeSelectionSet(
  const cgqlContainer<cgqlSPtr<Field>>& fields,
  SelectionSet& mergedSelectionSet
);

template<typename T>
Data completeValue(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<Object>>& source
);

template<typename T>
Data executeField(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<T>& fieldType,
  const SelectionSet& fields,
  const std::optional<cgqlSPtr<Object>>& source
);

cgqlUPtr<Object> executeSelectionSet(
  const ExecutionContext& ctx,
  const SelectionSet &selectionSet,
  const cgqlSPtr<ObjectTypeDefinition> &obj,
  const std::optional<cgqlSPtr<Object>>& source
);

cgqlUPtr<Object> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query,
  const cgqlSPtr<Schema>& schema
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

cgqlUPtr<Object> execute(
  const cgqlSPtr<internal::Schema>& schema,
  const internal::Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
