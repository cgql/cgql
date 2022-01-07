#ifndef EXECUTE_H
#define EXECUTE_H

#include "cgql/cgqlPch.h"
#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/type/Document.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

const GraphQLField& findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const std::string& fieldName
);

GroupedField collectFields(
  const GraphQLObject &objectType,
  const SelectionSet &selectionSet
);

SelectionSet mergeSelectionSet(
  const cgqlContainer<Field>& fields
);

template<typename T>
Data coerceLeafValue(
  const GraphQLScalarTypes& fieldType,
  const Data& data
);

Data coerceVariedLeafValue(
  const GraphQLScalarTypes& fieldType,
  const Data& data
);

Data completeValue(
  const GraphQLScalarTypes& fieldType,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
);

Data executeField(
  const GraphQLField& field,
  const GraphQLScalarTypes& fieldType,
  const cgqlContainer<Field>& fields,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
);

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
);

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema,
  const ResolverMap& resolverMap
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

ResultMap execute(
  const GraphQLSchema& schema,
  const internal::Document& document,
  const ResolverMap& resolverMap
);

}
#endif
