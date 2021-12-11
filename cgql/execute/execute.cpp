#include "execute.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

GraphQLField findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const string& fieldName
) {
  for(const GraphQLField& field : objectType.getFields()) {
    if(fieldName == field.getName()) {
      return field;
    }
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
  if(fieldType.index() == 2) {
    shared_ptr<GraphQLObject> obj =
      std::get<shared_ptr<GraphQLObject>>(result);
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
  GraphQLReturnTypes result = field.getResolver().has_value() ?
    field.getResolver().value()() : 0;
  return completeValue(fieldType, fields, result);
}

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType
) {
  ResultMap resultMap;
  GroupedField groupedFieldSet = cgql::internal::collectFields(
    objectType,
    selectionSet
  );
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    try {
      GraphQLField field =
        cgql::internal::findGraphQLFieldByName(
          objectType,
          responseKey
        );
      GraphQLScalarTypes fieldType = field.getType();
      resultMap.data.insert({
        responseKey,
        cgql::internal::executeField(field, fieldType, fields)
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
  return cgql::internal::executeSelectionSet(selection, queryType);
}

OperationDefinition getOperation(
  const Document& document,
  OperationType operationName
) {
  for(auto def : document.getDefinitions()) {
    if(def.index() == 0) {
      OperationDefinition opDef = std::get<OperationDefinition>(def);
      if(opDef.getOperationType() == operationName) {
        return opDef;
      }
    }
  }
  throw operationName;
}
} // internal

ResultMap execute(
  const GraphQLSchema &schema,
  const internal::Document &document
) {
  // get operation
  internal::OperationDefinition operation =
    cgql::internal::getOperation(document);
  return internal::executeQuery(operation, schema);
}

} // cgql
