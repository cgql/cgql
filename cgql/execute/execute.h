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

template<typename T>
const FieldTypeDefinition& findGraphQLFieldByName(
  const cgqlSPtr<T>& objectType,
  const std::string& fieldName
);

template<typename T>
void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedFields
);

void mergeSelectionSet(
  const cgqlContainer<cgqlSPtr<Field>>& fields,
  SelectionSet& mergedSelectionSet
);

Data coerceLeafValue(
  const cgqlSPtr<TypeDefinition>& fieldType,
  const Data& data
);

Data completeValue(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source
);

Data executeField(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const std::optional<cgqlSPtr<ResultMap>>& source
);

template<typename T>
cgqlUPtr<ResultMap> executeSelectionSet(
  const ExecutionContext& ctx,
  const SelectionSet &selectionSet,
  const cgqlSPtr<T> &objectType,
  const std::optional<cgqlSPtr<ResultMap>>& source
);

cgqlUPtr<ResultMap> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query,
  const cgqlSPtr<Schema>& schema
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

cgqlUPtr<ResultMap> execute(
  const cgqlSPtr<internal::Schema>& schema,
  const internal::Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
