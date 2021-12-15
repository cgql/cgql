#include "execute.h"
#include "cgql/logger/logger.h"
#include "defaultResolver.h"

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
        Field* field =
          std::get<Field*>(arg);

        string responseKey = field->getName();
  
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
  const Data& result,
  const std::optional<ResultMap>& source
) {
  Data completedValue;
  if(fieldType.index() == 2) {
    cgqlSPtr<ResultMap> v =
      std::get<cgqlSPtr<ResultMap>>(result);
    GraphQLObject obj;
    cgqlSPtr<GraphQLObject> schemaObj =
      std::get<cgqlSPtr<GraphQLObject>>(fieldType);
    for(auto const& elem : v->data) {
      for(auto field : schemaObj->getFields()) {
        if(field.getName() == elem.first) {
          obj.getMutableFields().push_back({
            elem.first,
            field.getType()
          });
        }
      }
    }
    SelectionSet mergedSelectionSet =
      mergeSelectionSet(fields);
    completedValue = cgqlSMakePtr<ResultMap>(
      executeSelectionSet(
        mergedSelectionSet,
        obj,
        *v
      )
    );
  } else {
    completedValue = result;
  }
  return completedValue;
}

Data executeField(
  const GraphQLField& field,
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields,
  const std::optional<ResultMap>& source
) {
  Data result;
  bool hasResolver = field.getResolver().has_value();
  if(hasResolver) {
    result = field.getResolver().value()();
  } else {
    result = defaultFieldResolver(source.value(), field.getName());
  }
  return completeValue(
    fieldType,
    fields,
    result,
    source
  );
}

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType,
  const std::optional<ResultMap>& source 
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
        executeField(
          field,
          fieldType,
          fields,
          source
        )
      });
    } catch(string& fieldName) {
      // logger::error(fieldName);
    }
  }
  return resultMap;
}

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema
) {
  const GraphQLObject& queryType = schema.getQuery();
  const SelectionSet& selection = query.getSelectionSet();
  return cgql::internal::executeSelectionSet(selection, queryType, {});
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
