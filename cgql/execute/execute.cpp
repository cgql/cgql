#include "cgql/execute/execute.h"
#include "cgql/execute/defaultResolver.h"
#include "cgql/logger/logger.h"
#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

const FieldTypeDefinition& findGraphQLFieldByName(
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const std::string& fieldName
) {
  for(auto const& field : objectType->getFields()) {
    if(fieldName == field.getName()) {
      return field;
    }
  }
  std::string msg;
  msg += "Field with name ";
  msg += fieldName;
  msg += " cannot be found in object ";
  msg += objectType->getName();
  cgqlAssert(true, msg.c_str());
  /* silence compiler warning */ return objectType->getFields()[0];
}

GroupedField collectFields(
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
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

Data coerceLeafValue(
  const cgqlSPtr<TypeDefinition>& fieldType,
  const Data& data
) {
  cgqlAssert(data.index() == 4, "Result cannot be null");
  /* const GraphQLTypesBase<T>& type =
    fromVariant<GraphQLTypesBase<T>>(fieldType);
  const GraphQLReturnTypes& variedValue =
    fromVariant<GraphQLReturnTypes>(data);
  const T& value = fromVariant<T>(variedValue);
  return type.getSerializer()(value); */
  return data;
}

Data coerceVariedLeafValue(
  const cgqlSPtr<TypeDefinition>& fieldType,
  const Data& data
) {
  switch(fieldType->getType()) {
    case INT_TYPE:
    case STRING_TYPE:
      return coerceLeafValue(fieldType, data);
    default:
      cgqlAssert(true, "Unable to coerce resolved value");
  }
  /* silence compiler warning */ return (GraphQLReturnTypes)0;
}

template<typename T>
Data completeListItem(
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const FieldTypeDefinition& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
) {
  const cgqlContainer<T>& rawResultList =
    fromVariant<cgqlContainer<T>>(result);
  cgqlContainer<T> resultList;
  resultList.reserve(rawResultList.size());
  for(auto const& rawResult : rawResultList) {
    resultList.push_back(
      fromVariant<T>(completeValue(
        fieldType->getInnerType(),
        field,
        fields,
        rawResult,
        source,
        resolverMap
      ))
    );
  }
  return resultList;
}

Data completeList(
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const FieldTypeDefinition& field,
  const cgqlContainer<Field>& fields,
  const Data& rawResult,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
) {
  switch(rawResult.index()) {
    case 2:
      return completeListItem<GraphQLReturnTypes>(
        fieldType,
        field,
        fields,
        rawResult,
        source,
        resolverMap
      );
      break;
    case 3:
      return completeListItem<cgqlSPtr<ResultMap>>(
        fieldType,
        field,
        fields,
        rawResult,
        source,
        resolverMap
      );
      break;
    default:
      cgqlAssert(true, "Unknown variant type");
  }
  return 0;
}

Data completeValue(
  const cgqlSPtr<TypeDefinition>& fieldType,
  const FieldTypeDefinition& field,
  const cgqlContainer<Field>& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
) {
  if(fieldType->getType() == NON_NULL_TYPE) {
    const cgqlSPtr<NonNullTypeDefinition<TypeDefinition>>& nonNull =
      std::static_pointer_cast<NonNullTypeDefinition<TypeDefinition>>(fieldType);
    if(result.index() == 4) {
      // field error
    }
    return completeValue(
      nonNull->getInnerType(),
      field,
      fields,
      result,
      source,
      resolverMap
    );
  }
  if(result.index() == 4) {
    return std::monostate{};
  }
  if(fieldType->getType() == LIST_TYPE) {
    const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& list =
      std::static_pointer_cast<ListTypeDefinition<TypeDefinition>>(fieldType);
    return completeList(
      list,
      field,
      fields,
      result,
      source,
      resolverMap
    );
  }
  if(fieldType->getType() == DefinitionType::OBJECT_TYPE) {
    const cgqlSPtr<ResultMap>& v =
      fromVariant<cgqlSPtr<ResultMap>>(result);
    const cgqlSPtr<ObjectTypeDefinition>& schemaObj =
      std::static_pointer_cast<ObjectTypeDefinition>(fieldType);

    SelectionSet mergedSelectionSet =
      mergeSelectionSet(fields);
    return executeSelectionSet(
      mergedSelectionSet,
      schemaObj,
      v,
      resolverMap
    );
  } else {
    return coerceVariedLeafValue(fieldType, result);
  }
}

Args buildArgumentMap(
  const Field& field,
  const FieldTypeDefinition& fieldType
) {
  Args arg;
  const cgqlContainer<Argument>& argumentValues =
    field.getArgs();
  const cgqlContainer<ArgumentTypeDefinition>& argumentDefinitions =
    fieldType.getArgs();
  for(auto const& argDef : argumentDefinitions) {
    const std::string& argName = argDef.getName();
    const cgqlSPtr<TypeDefinition>& argType = argDef.getType();
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
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const cgqlContainer<Field>& fields,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
) {
  Data result;
  const ResolverMap::const_iterator& it = resolverMap.find(field.getName());
  if(it != resolverMap.end()) {
    result = it->second(buildArgumentMap(
      fields[0],
      field
    ));
  } else {
    auto defaultResolvedValue = defaultFieldResolver(
      source.value(),
      field.getName()
    );
    result = defaultResolvedValue.has_value() ?
      defaultResolvedValue.value() :
      std::monostate{};
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

cgqlSPtr<ResultMap> executeSelectionSet(
  const SelectionSet &selectionSet,
  const cgqlSPtr<ObjectTypeDefinition> &objectType,
  const std::optional<cgqlSPtr<ResultMap>>& source,
  const ResolverMap& resolverMap
) {
  ResultMap resultMap;
  GroupedField groupedFieldSet = collectFields(
    objectType,
    selectionSet
  );
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    const FieldTypeDefinition& field = findGraphQLFieldByName(
      objectType,
      fields[0].getName()
    );
    const cgqlSPtr<TypeDefinition>& fieldType = field.getType();
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
  return cgqlSMakePtr<ResultMap>(resultMap);
}

cgqlSPtr<ResultMap> executeQuery(
  const OperationDefinition& query,
  const cgqlSPtr<Schema>& schema,
  const ResolverMap& resolverMap
) {
  const cgqlSPtr<ObjectTypeDefinition>& queryType = schema->getQuery();
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

cgqlSPtr<ResultMap> execute(
  const cgqlSPtr<internal::Schema> &schema,
  const internal::Document &document,
  const ResolverMap& resolverMap
) {
  // get operation
  const internal::OperationDefinition& operation = internal::getOperation(document);
  return internal::executeQuery(
    operation,
    schema,
    resolverMap
  );
}

} // cgql
