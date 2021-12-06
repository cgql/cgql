#ifndef EXECUTE_H
#define EXECUTE_H

#include "../type/Document.h"
#include "../schema/GraphQLDefinition.h"
#include <unordered_map>

using std::unordered_map;

typedef unordered_map<string, vector<Field>> GroupedField;
typedef unordered_map<string, GraphQLOutputTypes> ResultMap;

GraphQLField& findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const string& fieldName
);

GroupedField collectFields(
  const GraphQLObject& objectType,
  const SelectionSet& selectionSet
);

GraphQLOutputTypes executeField(
  const GraphQLField& field,
  const GraphQLType& fieldType,
  const vector<Field>& fields
);

ResultMap executeSelectionSet(
  const SelectionSet& selectionSet,
  const GraphQLObject& objectType
);

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema
);
OperationDefinition getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);
ResultMap execute(
  const GraphQLSchema& schema,
  const Document& document
);

#endif
