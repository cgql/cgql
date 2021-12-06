#include "execute.h"
#include "cgql/logger/logger.h"
#include <variant>

using std::visit;
using std::holds_alternative;

GraphQLField& findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const string& fieldName
) {
  for(GraphQLField& field : objectType.getFields()) {
    if(fieldName == field.getName())
      return field;
  }
  throw fieldName;
}

GroupedField collectFields(
  const GraphQLObject &objectType,
  const SelectionSet &selectionSet
) {
  GroupedField groupedFields;
  for(Selection selection : selectionSet) {
    std::visit([&groupedFields](Selection&& arg) {
      if(std::holds_alternative<Field*>(arg)) {
        // holds a Field*
        Field* field = std::get<Field*>(arg);

        const string& responseKey = field->getName();

        GroupedField::iterator it =
          groupedFields.find(responseKey);
        if(it != groupedFields.end()) {
          it->second.push_back(*field);
        } else {
          groupedFields.insert({ responseKey, { *field } });
        }

        delete field;
      }
    }, selection);
  }
  return groupedFields;
}

GraphQLOutputTypes executeField(
  const GraphQLField& field,
  const GraphQLType& fieldType,
  const vector<Field>& fields
) {
  GraphQLOutputTypes result = field.getResolver()();
  return result;
}

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType
) {
  ResultMap resultMap;
  GroupedField groupedFieldSet = collectFields(
    objectType,
    selectionSet
  );
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    const string& fieldName = fields[0].getName();
    try {
      const GraphQLField& field =
        findGraphQLFieldByName(
          objectType,
          fieldName
        );
      const GraphQLType& fieldType = field.getType();
      resultMap.insert({
        responseKey,
        executeField(
          field,
          fieldType,
          fields
        )
      });
    } catch (string& fieldName) {}
  }
  return resultMap;
}

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema
) {
  const GraphQLObject& queryType = schema.getQuery();
  const SelectionSet& selection = query.getSelectionSet();
  return executeSelectionSet(selection, queryType);
}

OperationDefinition getOperation(
  const Document& document,
  OperationType operationName
) {
  for(auto opDef : document.getDefinitions()) {
    if(opDef.getOperationType() == operationName) {
      return opDef;
    }
  }
  throw operationName;
}

ResultMap execute(const GraphQLSchema &schema, const Document &document) {
  // get operation
  OperationDefinition operation = getOperation(document);
  return executeQuery(operation, schema);
}
