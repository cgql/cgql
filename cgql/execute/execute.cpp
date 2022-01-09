#include "cgql/execute/execute.h"
#include "cgql/execute/defaultResolver.h"
#include "cgql/logger/logger.h"
#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

const GraphQLField& findGraphQLFieldByName(
  const GraphQLObject& objectType,
  const std::string& fieldName
) {
  for(auto const& field : objectType.getFields()) {
    if(fieldName == field.getName()) {
      return field;
    }
  }
  std::string msg;
  msg += "Field with name ";
  msg += fieldName;
  msg += " cannot be found in object ";
  msg += objectType.getName();
  cgqlAssert(true, msg.c_str());
  /* silence compiler warning */ return objectType.getFields()[0];
}

GroupedField collectFields(
  const GraphQLObject &objectType,
  const SelectionSet &selectionSet
) {
  GroupedField groupedFields;
  for(auto const& selection : selectionSet) {
    if(selection.index() == 0) {
      // holds a Field*
      const cgqlSPtr<Field>& field =
        fromVariant<cgqlSPtr<Field>>(selection);

      const std::string& responseKey =
        field->getAlias().empty() ?
          field->getName() : field->getAlias();

      const GroupedField::iterator& it =
        groupedFields.find(responseKey);
      if(it != groupedFields.end()) {
        it->second.push_back(*field);
      } else {
        cgqlContainer<Field> fields;
        fields.push_back(*field);
        groupedFields.try_emplace(
          responseKey, fields
        );
      }
    }
  }
  return groupedFields;
}

SelectionSet mergeSelectionSet(
  const cgqlContainer<Field>& fields
) {
  SelectionSet mergedSelectionSet;
  for(auto const& field : fields) {
    const SelectionSet& fieldSelectionSet = field.getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(auto const& subField : fieldSelectionSet) {
      mergedSelectionSet.push_back(subField);
    }
  }
  return mergedSelectionSet;
}

template<typename T>
Data coerceLeafValue(
  const GraphQLScalarTypes& fieldType,
  const Data& data
) {
  const GraphQLTypesBase<T>& type =
    fromVariant<GraphQLTypesBase<T>>(fieldType);
  const GraphQLReturnTypes& variedValue =
    fromVariant<GraphQLReturnTypes>(data);
  const T& value = fromVariant<T>(variedValue);
  return type.serialize(value);
}

Data coerceVariedLeafValue(
  const GraphQLScalarTypes& fieldType,
  const Data& data
) {
  switch(fieldType.index()) {
    case 0:
      return coerceLeafValue<Int>(fieldType, data);
      break;
    case 1:
      return coerceLeafValue<String>(fieldType, data);
      break;
    default:
      cgqlAssert(true, "Unknown variant type");
  }
  /* silence compiler warning */ return 0;
}

template<typename T>
Data completeListItem(
  const GraphQLScalarTypes& fieldType,
  const GraphQLField& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  const cgqlContainer<T>& rawResultList =
    fromVariant<cgqlContainer<T>>(result);
  cgqlContainer<T> resultList;
  resultList.reserve(rawResultList.size());
  for(auto const& rawResult : rawResultList) {
    resultList.push_back(
      fromVariant<T>(completeValue(
        fieldType,
        field,
        fields,
        rawResult,
        source,
        resolverMap
      ))
    );
  }
  return std::move(resultList);
}

Data completeList(
  const GraphQLScalarTypes& fieldType,
  const GraphQLField& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  switch(result.index()) {
    case 2:
      return completeListItem<GraphQLReturnTypes>(
        fieldType,
        field,
        fields,
        result,
        source,
        resolverMap
      );
    case 3:
      return completeListItem<cgqlSPtr<ResultMap>>(
        fieldType,
        field,
        fields,
        result,
        source,
        resolverMap
      );
    default:
      cgqlAssert(true, "Unknown variant type");
  }
  return 0;
}

Data completeValue(
  const GraphQLScalarTypes& fieldType,
  const GraphQLField& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  if(isList(result) && field.getTypeMetaData().isList()) {
    return completeList(
      fieldType,
      field,
      fields,
      result,
      source,
      resolverMap
    );
  }
  if(fieldType.index() == 2) {
    const cgqlSPtr<ResultMap>& v =
      fromVariant<cgqlSPtr<ResultMap>>(result);
    const cgqlSPtr<GraphQLObject>& schemaObj =
      fromVariant<cgqlSPtr<GraphQLObject>>(fieldType);

    SelectionSet mergedSelectionSet =
      mergeSelectionSet(fields);
    return cgqlSMakePtr<ResultMap>(
      executeSelectionSet(
        mergedSelectionSet,
        *schemaObj,
        *v,
        resolverMap
      )
    );
  } else {
    return coerceVariedLeafValue(fieldType, result);
  }
}

Args buildArgumentMap(
  const Field& field,
  const GraphQLField& fieldType
) {
  Args arg;
  const cgqlContainer<Argument>& argumentValues =
    field.getArgs();
  const cgqlContainer<GraphQLArgument>& argumentDefinitions =
    fieldType.getArgs();
  for(auto const& argDef : argumentDefinitions) {
    const std::string& argName = argDef.getName();
    const GraphQLScalarTypes& argType = argDef.getType();
    const auto& it = std::find_if(
      argumentValues.begin(),
      argumentValues.end(),
      [&argName](const Argument& arg) {
        return arg.getName() == argName;
      }
    );
    bool hasValue = it != argumentValues.end();
    const Arg& argValue = it->getValue();
    if(hasValue) {
      arg.argsMap.try_emplace(
        argName,
        argValue
      );
    }
  }
  return arg;
}

Data executeField(
  const GraphQLField& field,
  const GraphQLScalarTypes& fieldType,
  const cgqlContainer<Field>& fields,
  const std::optional<ResultMap>& source,
  const ResolverMap& resolverMap
) {
  Data result;
  auto it = resolverMap.find(field.getName());
  if(it != resolverMap.end()) {
    result = it->second(buildArgumentMap(
      fields[0],
      field
    ));
  } else {
    result = defaultFieldResolver(
      source.value(),
      field.getName()
    );
  }
  return completeValue(
    fieldType,
    field,
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
  GroupedField groupedFieldSet = collectFields(
    objectType,
    selectionSet
  );
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    GraphQLField field = findGraphQLFieldByName(
        objectType,
        fields[0].getName()
      );
    const GraphQLScalarTypes& fieldType = field.getType();
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
  return executeSelectionSet(
    selection,
    queryType,
    {},
    resolverMap
  );
}

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName
) {
  for(auto const& def : document.getDefinitions()) {
    if(def.index() == 0) {
      const OperationDefinition& opDef = fromVariant<OperationDefinition>(def);
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
  auto operation = internal::getOperation(document);
  return internal::executeQuery(
    operation,
    schema,
    resolverMap
  );
}

} // cgql
