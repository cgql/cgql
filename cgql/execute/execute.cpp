#include "cgql/execute/execute.h"
#include "cgql/execute/defaultResolver.h"
#include "cgql/logger/logger.h"
#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

template<typename T>
const FieldTypeDefinition& findGraphQLFieldByName(
  const cgqlSPtr<T>& objectType,
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

template<typename T>
void collectFields(
  const cgqlSPtr<T>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedFields
) {
  for(auto const& selection : selectionSet) {
    SelectionType type = selection->getSelectionType();
    if(type == SelectionType::FIELD) {
      // holds a Field*
      const cgqlSPtr<Field>& field =
        std::static_pointer_cast<Field>(selection);

      const std::string& responseKey =
        field->getAlias().empty() ?
          field->getName() : field->getAlias();

      const GroupedField::iterator& it =
        groupedFields.find(responseKey);
      if(it != groupedFields.end()) {
        it->second.emplace_back(field);
      } else {
        SelectionSet fields;
        fields.reserve(1);
        fields.emplace_back(field);
        groupedFields.try_emplace(
          responseKey, std::move(fields)
        );
      }
    } else if(type == SelectionType::INLINE_FRAGMENT) {
      const cgqlSPtr<InlineFragment>& inlineFragment =
        std::static_pointer_cast<InlineFragment>(selection);
      const std::string& typeCondition = inlineFragment->getTypeCondition();
      
      bool shouldNotSkip = [&objectType, &typeCondition]() {
        if(typeCondition == objectType->getName()) {
          return true;
        }
        return false;
      }();
      if(!shouldNotSkip) continue;

      const SelectionSet& selectionSet = inlineFragment->getSelectionSet();
      collectFields(objectType, selectionSet, groupedFields);
    }
  }
}

template<typename T>
void collectSubFields(
  const cgqlSPtr<T>& objectType,
  const SelectionSet& selectionSet,
  GroupedField& groupedFields
) {
  for(auto const& selection : selectionSet) {
    collectFields(
      objectType,
      selection->getSelectionSet(),
      groupedFields
    );
  }
}

void mergeSelectionSet(
  const SelectionSet& fields,
  SelectionSet& mergedSelectionSet
) {
  for(auto const& field : fields) {
    const SelectionSet& fieldSelectionSet = field->getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(auto const& subField : fieldSelectionSet) {
      mergedSelectionSet.emplace_back(subField);
    }
  }
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
  return std::move(data);
}

template<typename T>
Data completeListItem(
  const ExecutionContext& ctx,
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  const cgqlContainer<T>& rawResultList =
    fromVariant<cgqlContainer<T>>(result);
  cgqlContainer<T> resultList;
  resultList.reserve(rawResultList.size());
  for(auto const& rawResult : rawResultList) {
    resultList.emplace_back(
      fromVariant<T>(completeValue(
        ctx,
        fieldType->getInnerType(),
        field,
        fields,
        rawResult,
        source
      ))
    );
  }
  return resultList;
}

Data completeList(
  const ExecutionContext& ctx,
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& rawResult,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  switch(rawResult.index()) {
    case 2:
      return completeListItem<GraphQLReturnTypes>(
        ctx,
        fieldType,
        field,
        fields,
        rawResult,
        source
      );
      break;
    case 3:
      return completeListItem<cgqlSPtr<ResultMap>>(
        ctx,
        fieldType,
        field,
        fields,
        rawResult,
        source
      );
      break;
    default:
      cgqlAssert(true, "Unknown variant type");
  }
  return 0;
}

template<typename T>
static cgqlUPtr<ResultMap> executeGroupedFieldSet(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& objectType,
  const GroupedField& groupedFieldSet,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  cgqlUPtr<ResultMap> resultMap = cgqlUMakePtr<ResultMap>();
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    const FieldTypeDefinition& field = findGraphQLFieldByName(
      objectType,
      responseKey
    );
    const cgqlSPtr<TypeDefinition>& fieldType = field.getType();
    resultMap->data.try_emplace(
      responseKey,
      executeField(
        ctx,
        field,
        fieldType,
        fields,
        source
      )
    );
  }
  return resultMap;
}

template<typename T>
static Data executeInnerSelectionSet(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& objectType,
  const Data& result,
  const SelectionSet& fields
) {
  const cgqlSPtr<ResultMap>& v =
    fromVariant<cgqlSPtr<ResultMap>>(result);
  SelectionSet mergedSelectionSet;
  mergeSelectionSet(fields, mergedSelectionSet);
  return executeSelectionSet(
    ctx,
    mergedSelectionSet,
    objectType,
    v
  );
}

template<typename T>
static Data completeAbstractType(
  const ExecutionContext& ctx,
  const T& fieldType,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  const cgqlSPtr<ResultMap>& resultMap =
    fromVariant<cgqlSPtr<ResultMap>>(result);
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& possibleTypes =
    ctx.schema->getPossibleTypes(fieldType);
  const TypeOfMap::const_iterator& it = ctx.typeOfMap->find(fieldType->getName());
  for(auto const& possibleType : possibleTypes) {
    String typeName = it->second(resultMap);
    if(possibleType->getName() == typeName) {
      cgqlAssert(
        possibleType->getType() != DefinitionType::OBJECT_TYPE,
        "Type needs to he an object"
      );
      const cgqlSPtr<ObjectTypeDefinition>& object =
        std::static_pointer_cast<ObjectTypeDefinition>(possibleType);
      GroupedField groupedFields;
      collectSubFields(object, fields, groupedFields);
      return executeGroupedFieldSet(
        ctx,
        object,
        groupedFields,
        resultMap
      );
    }
  }
  cgqlAssert(true, "Unable to resolve value for implementation of interface");
  /* silence compiler warning */ return 0;
}

Data completeValue(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  DefinitionType type = fieldType->getType();
  if(type == NON_NULL_TYPE) {
    const cgqlSPtr<NonNullTypeDefinition<TypeDefinition>>& nonNull =
      std::static_pointer_cast<NonNullTypeDefinition<TypeDefinition>>(fieldType);
    if(result.index() == 4) {
      // field error
    }
    return completeValue(
      ctx,
      nonNull->getInnerType(),
      field,
      fields,
      result,
      source
    );
  }
  if(result.index() == 4) {
    return std::monostate{};
  }
  if(type == LIST_TYPE) {
    const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& list =
      std::static_pointer_cast<ListTypeDefinition<TypeDefinition>>(fieldType);
    return completeList(
      ctx,
      list,
      field,
      fields,
      result,
      source
    );
  }
  if(type == DefinitionType::OBJECT_TYPE) {
    const cgqlSPtr<ObjectTypeDefinition>& schemaObj =
      std::static_pointer_cast<ObjectTypeDefinition>(fieldType);

    return executeInnerSelectionSet<ObjectTypeDefinition>(
      ctx,
      schemaObj,
      result,
      fields
    );
  }
  if(type == DefinitionType::INTERFACE_TYPE) {
    const cgqlSPtr<InterfaceTypeDefinition>& schemaObj =
      std::static_pointer_cast<InterfaceTypeDefinition>(fieldType);
    return completeAbstractType(ctx, schemaObj, fields, result, source);
  }
  return coerceLeafValue(fieldType, result);
}

Args buildArgumentMap(
  const cgqlSPtr<Selection>& selection,
  const FieldTypeDefinition& fieldType
) {
  Args arg;
  const cgqlSPtr<Field>& field =
    std::static_pointer_cast<Field>(selection);
  const cgqlContainer<Argument>& argumentValues =
    field->getArgs();
  const cgqlContainer<ArgumentTypeDefinition>& argumentDefinitions =
    fieldType.getArgs();
  for(auto const& argDef : argumentDefinitions) {
    const std::string& argName = argDef.getName();
    const auto& it = std::find_if(
      argumentValues.begin(),
      argumentValues.end(),
      [&argName](const Argument& arg) {
        return arg.getName() == argName;
      }
    );
    bool hasValue = it != argumentValues.end();
    if(hasValue) {
      arg.argsMap.try_emplace(
        argName,
        it->getValue()
      );
    }
  }
  return arg;
}

Data executeField(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  const auto& it = ctx.resolverMap->find(field.getName());
  Data result = [&]() {
    if(it != ctx.resolverMap->end()) {
      return it->second(buildArgumentMap(
        fields[0],
        field
      ));
    } else {
      auto defaultResolvedValue = defaultFieldResolver(
        source.value(),
        field.getName()
      );
      return defaultResolvedValue.has_value() ?
        defaultResolvedValue.value() :
        std::monostate{};
    }
  }();
  return completeValue(
    ctx,
    fieldType,
    field,
    fields,
    result,
    source
  );
}

template<typename T>
cgqlUPtr<ResultMap> executeSelectionSet(
  const ExecutionContext& ctx,
  const SelectionSet &selectionSet,
  const cgqlSPtr<T> &objectType,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  const cgqlSPtr<ObjectTypeDefinition>& obj =
    std::static_pointer_cast<ObjectTypeDefinition>(objectType);
  GroupedField groupedFieldSet;
  collectFields(
    obj,
    selectionSet,
    groupedFieldSet
  );
  return executeGroupedFieldSet(
    ctx,
    obj,
    groupedFieldSet,
    source
  );
}

cgqlUPtr<ResultMap> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query
) {
  const cgqlSPtr<ObjectTypeDefinition>& queryType = ctx.schema->getQuery();
  const SelectionSet& selection = query.getSelectionSet();
  return executeSelectionSet(
    ctx,
    selection,
    queryType,
    {}
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

cgqlUPtr<ResultMap> execute(
  const cgqlSPtr<internal::Schema> &schema,
  const internal::Document &document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  // get operation
  const internal::OperationDefinition& operation = internal::getOperation(document);
  internal::ExecutionContext ctx;
  ctx.schema = schema;
  ctx.resolverMap = cgqlUMakePtr<ResolverMap>(resolverMap);
  ctx.typeOfMap = cgqlUMakePtr<TypeOfMap>(typeOfMap);
  return internal::executeQuery(
    ctx,
    operation
  );
}

} // cgql
