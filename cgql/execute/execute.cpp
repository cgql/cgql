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
    if(selection.index() == 0) {
      // holds a Field*
      cgqlSPtr<Field> field =
        std::get<cgqlSPtr<Field>>(selection);

      string responseKey =
        field->getAlias().empty() ?
          field->getName() : field->getAlias();

      GroupedField::iterator it =
        groupedFields.find(responseKey);
      if(it != groupedFields.end()) {
        it->second.push_back(*field);
      } else {
        std::vector<Field> fields { *field };
        groupedFields.try_emplace(
          responseKey, fields
        );
      }
    }
  }
  return groupedFields;
}

SelectionSet mergeSelectionSet(
  const vector<Field>& fields
) {
  SelectionSet mergedSelectionSet;
  for(const Field& field : fields) {
    const SelectionSet& fieldSelectionSet = field.getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(const Selection& subField : fieldSelectionSet) {
      mergedSelectionSet.push_back(subField);
    }
  }
  return mergedSelectionSet;
}

Data completeValue(
  const GraphQLScalarTypes& fieldType,
  const vector<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  Data completedValue;
  if(fieldType.index() == 2) {
    cgqlSPtr<ResultMap> v =
      std::get<cgqlSPtr<ResultMap>>(result);
    GraphQLObject obj;
    cgqlSPtr<GraphQLObject> schemaObj =
      std::get<cgqlSPtr<GraphQLObject>>(fieldType);
    for(auto const& elem : v->data) {
      for(const GraphQLField& field : schemaObj->getFields()) {
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
        *v,
        resolverMap
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
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  Data result;
  auto it = resolverMap.find(field.getName());
  if(it != resolverMap.end()) {
    result = it->second();
  } else {
    result = defaultFieldResolver(
      source.value(),
      field.getName()
    );
  }
  return completeValue(
    fieldType,
    fields,
    result,
    source,
    resolverMap
  );
}

ResultMap executeSelectionSet(
  const SelectionSet &selectionSet,
  const GraphQLObject &objectType,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
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
          fields[0].getName()
        );
      GraphQLScalarTypes fieldType = field.getType();
      resultMap.data.try_emplace(
        responseKey,
        executeField(
          field,
          fieldType,
          fields,
          source,
          resolverMap
        )
      );
    } catch(string& fieldName) {
      // logger::error(fieldName);
    }
  }
  return resultMap;
}

ResultMap executeQuery(
  OperationDefinition& query,
  const GraphQLSchema& schema,
  const ResolverMap& resolverMap

) {
  const GraphQLObject& queryType = schema.getQuery();
  const SelectionSet& selection = query.getSelectionSet();
  return cgql::internal::executeSelectionSet(
    selection,
    queryType,
    {},
    resolverMap
  );
}

OperationDefinition getOperation(
  const Document& document,
  OperationType operationName
) {
  for(const Definition& def : document.getDefinitions()) {
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
  const internal::Document &document,
  const ResolverMap& resolverMap
) {
  // get operation
  internal::OperationDefinition operation =
    cgql::internal::getOperation(document);
  return internal::executeQuery(
    operation,
    schema,
    resolverMap
  );
}

} // cgql
