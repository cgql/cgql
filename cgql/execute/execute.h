#ifndef EXECUTE_H
#define EXECUTE_H

#include "cgql/cgqlPch.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/type/Document.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

const FieldTypeDefinition& findGraphQLFieldByName(
  const ObjectTypeDefinition& objectType,
  const std::string& fieldName
);

GroupedField collectFields(
  const ObjectTypeDefinition &objectType,
  const SelectionSet &selectionSet
);

SelectionSet mergeSelectionSet(
  const cgqlContainer<Field>& fields
);

Data coerceLeafValue(
  const TypeDefinition& fieldType,
  const Data& data
);

Data coerceVariedLeafValue(
  const TypeDefinition& fieldType,
  const Data& data
);

Data completeValue(
  const TypeDefinition& fieldType,
  const FieldTypeDefinition& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
);

Data executeField(
  const FieldTypeDefinition& field,
  const TypeDefinition& fieldType,
  const cgqlContainer<Field>& fields,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
);

cgqlSPtr<ResultMap> executeSelectionSet(
  const SelectionSet &selectionSet,
  const cgqlSPtr<ObjectTypeDefinition> &objectType,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
);

cgqlSPtr<ResultMap> executeQuery(
  const OperationDefinition& query,
  const Schema& schema,
  const ResolverMap& resolverMap
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

cgqlSPtr<ResultMap> execute(
  const internal::Schema& schema,
  const internal::Document& document,
  const ResolverMap& resolverMap
);

}
#endif
