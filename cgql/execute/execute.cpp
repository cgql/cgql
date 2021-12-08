#include "execute.h"
#include "cgql/logger/logger.h"

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

SelectionSet mergeSelectionSet(
  const vector<Field>& fields
) {
  SelectionSet mergedSelectionSet;
  for(Field field : fields) {
    SelectionSet fieldSelectionSet = field.getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(Selection subField : fieldSelectionSet) {
      mergedSelectionSet.push_back(subField);
    }
  }
  return mergedSelectionSet;
}

Data completeValue(
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields,
  const GraphQLReturnTypes& result
) {
  Data completedValue;
  logger::success(fieldType.index());
  if(fieldType.index() == 3) {
    GraphQLObject* obj =
      std::get<GraphQLObject*>(result);
    SelectionSet mergedSelectionSet =
      mergeSelectionSet(fields);
    ResultMap resultingValue = executeSelectionSet(
      mergedSelectionSet,
      *obj
    );
    completedValue = std::make_shared<ResultMap>(resultingValue);
  } else {
    std::visit([&](GraphQLReturnTypes&& arg) {
      if(arg.index() == 0) {
        completedValue = std::get<Int>(arg);
      } else {
        completedValue = std::get<String>(arg);
      }
    }, result);
  }
  return completedValue;
}

Data executeField(
  const GraphQLField& field,
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields
) {
  GraphQLReturnTypes result = field.getResolver()();
  return completeValue(fieldType, fields, result);
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
    try {
      const GraphQLField& field =
        findGraphQLFieldByName(
          objectType,
          responseKey
        );
      const GraphQLScalarTypes& fieldType = field.getType();
      resultMap.data.insert({
        responseKey,
        executeField(field, fieldType, fields)
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
