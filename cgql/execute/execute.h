#ifndef EXECUTE_H
#define EXECUTE_H

#include "../cgqlPch.h"
#include "../schema/GraphQLDefinition.h"
#include "../type/Document.h"

namespace cgql {
namespace internal {

GraphQLField findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const string& fieldName
);

GroupedField collectFields(
  const GraphQLObject &objectType,
  const SelectionSet &selectionSet
);

SelectionSet mergeSelectionSet(
  const vector<Field>& fields
);

Data completeValue(
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source
);

Data executeField(
  const GraphQLField& field,
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields,
  const std::optional<ResultMap>& source
);

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType,
  const std::optional<ResultMap>& source 
);

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema
);

OperationDefinition getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

ResultMap execute(
  const GraphQLSchema& schema,
  const internal::Document& document
);

}
#endif
