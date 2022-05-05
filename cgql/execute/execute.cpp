#include "cgql/execute/execute.h"
#include "cgql/execute/defaultResolver.h"
#include "cgql/logger/logger.h"
#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

FieldTypeDefinition findGraphQLFieldByName(
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

void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedFields
) {
  for(cgqlSPtr<Selection> selection : selectionSet) {
    SelectionType type = selection->getSelectionType();
    if(type == SelectionType::FIELD) {
      // holds a Field*
      cgqlSPtr<Field> field =
        std::static_pointer_cast<Field>(selection);

      const std::string& responseKey = field->getResponseKey();

      GroupedField::iterator it =
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
      cgqlSPtr<InlineFragment> inlineFragment =
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
      collectFields(ctx, objectType, selectionSet, groupedFields);
    } else if(type == SelectionType::FRAGMENT) {
      cgqlSPtr<Fragment> fragment =
        std::static_pointer_cast<Fragment>(selection);
      cgqlContainer<FragmentDefinition>::const_iterator it =
        std::find_if(
          ctx.fragments.begin(),
          ctx.fragments.end(),
          [&fragment](const FragmentDefinition& fragmentDef) {
            return fragment->getName() == fragmentDef.getName();
          }
        );
      collectFields(ctx, objectType, it->getSelectionSet(), groupedFields);
    }
  }
}

template<typename T>
void collectSubFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& objectType,
  const SelectionSet& selectionSet,
  GroupedField& groupedFields
) {
  for(cgqlSPtr<Selection> selection : selectionSet) {
    collectFields(
      ctx,
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
  for(cgqlSPtr<Selection> field : fields) {
    const SelectionSet& fieldSelectionSet = field->getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(cgqlSPtr<Selection> subField : fieldSelectionSet) {
      mergedSelectionSet.emplace_back(subField);
    }
  }
}

Data coerceLeafValue(
  const cgqlSPtr<TypeDefinition>& fieldType,
  const Data& data
) {
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
  for(T rawResult : rawResultList) {
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

static cgqlUPtr<ResultMap> executeGroupedFieldSet(
  const ExecutionContext& ctx,
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const GroupedField& groupedFieldSet,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  cgqlUPtr<ResultMap> resultMap = cgqlUMakePtr<ResultMap>();
  resultMap->data.reserve(groupedFieldSet.size());

  for(auto const& [responseKey, fields] : groupedFieldSet) {
    const FieldTypeDefinition& field = findGraphQLFieldByName(
      objectType,
      std::static_pointer_cast<Field>(fields[0])->getName()
    );
    resultMap->data.try_emplace(
      responseKey,
      executeField(
        ctx,
        field,
        field.getType(),
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
  TypeOfMap::const_iterator it = ctx.typeOfMap.find(fieldType->getName());
  for(cgqlSPtr<TypeDefinition> possibleType : possibleTypes) {
    String typeName = it->second(resultMap);
    if(possibleType->getName() == typeName) {
      const cgqlSPtr<ObjectTypeDefinition>& object =
        std::static_pointer_cast<ObjectTypeDefinition>(possibleType);
      GroupedField groupedFields;
      collectSubFields(ctx, object, fields, groupedFields);
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

template<typename T>
Data completeValue(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  DefinitionType type = fieldType->getType();
  if(type == DefinitionType::NON_NULL_TYPE) {
    cgqlSPtr<NonNullTypeDefinition<TypeDefinition>> nonNull =
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
  switch (type) {
    case DefinitionType::LIST_TYPE: {
      cgqlSPtr<ListTypeDefinition<TypeDefinition>> list =
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
    case DefinitionType::OBJECT_TYPE: {
      cgqlSPtr<ObjectTypeDefinition> schemaObj =
        std::static_pointer_cast<ObjectTypeDefinition>(fieldType);

      return executeInnerSelectionSet<ObjectTypeDefinition>(
        ctx,
        schemaObj,
        result,
        fields
      );
    }
    case DefinitionType::INTERFACE_TYPE: {
      cgqlSPtr<InterfaceTypeDefinition> schemaObj =
        std::static_pointer_cast<InterfaceTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result, source);
    }
    case DefinitionType::UNION_TYPE: {
      cgqlSPtr<UnionTypeDefinition> schemaObj =
        std::static_pointer_cast<UnionTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result, source);
    }
    case DefinitionType::ENUM_TYPE: {
      cgqlSPtr<EnumTypeDefinition> enumType =
        std::static_pointer_cast<EnumTypeDefinition>(fieldType);
      return coerceLeafValue(enumType, result);
    }
    case DefinitionType::DEFAULT_WRAP: {
      cgqlSPtr<DefaultWrapTypeDefinition<TypeDefinition>> defaultWrap =
        std::static_pointer_cast<DefaultWrapTypeDefinition<TypeDefinition>>(fieldType);
      return completeValue(
        ctx,
        defaultWrap->getInnerType(),
        field,
        fields,
        result,
        source
      );
    }
    case DefinitionType::SCALAR_TYPE:
      return coerceLeafValue(fieldType, result);
    default:
      cgqlAssert(true, "TypeDef cannot be base");
  }
  /* silence compiler warning */ return 0;
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
    GraphQLInputTypes defaultValue = argDef.getDefaultValue();
    bool hasValue = it != argumentValues.end();
    if(!hasValue) {
      arg.argsMap.try_emplace(
        argName,
        defaultValue
      );
    } else if (
      argDef.getType()->getType() == DefinitionType::NON_NULL_TYPE &&
      !hasValue
    ) {
      cgqlAssert(true, "Value is null or not provided");
    } else {
      arg.argsMap.try_emplace(
        argName,
        it->getValue()
      );
    }
  }
  return arg;
}

template<typename T>
Data executeField(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<T>& fieldType,
  const SelectionSet& fields,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  ResolverMap::const_iterator it = ctx.resolverMap.find(field.getName());
  Data result = [&]() {
    if(it != ctx.resolverMap.end()) {
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

cgqlUPtr<ResultMap> executeSelectionSet(
  const ExecutionContext& ctx,
  const SelectionSet &selectionSet,
  const cgqlSPtr<ObjectTypeDefinition> &obj,
  const std::optional<cgqlSPtr<ResultMap>>& source
) {
  GroupedField groupedFieldSet;
  collectFields(
    ctx,
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
      const OperationDefinition& opDef =
        fromVariant<OperationDefinition>(def);
      if(opDef.getOperationType() == operationName) {
        return opDef;
      }
    }
  }
  throw operationName;
}

cgqlContainer<FragmentDefinition> getFragmentsFromQuery(const internal::Document& document) {
  cgqlContainer<FragmentDefinition> fragments;
  for(auto const& def : document.getDefinitions()) {
    if(def.index() == 1) {
      fragments.emplace_back(fromVariant<FragmentDefinition>(def));
    }
  }
  return fragments;
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
  ctx.resolverMap = resolverMap;
  ctx.typeOfMap = typeOfMap;
  ctx.fragments = internal::getFragmentsFromQuery(document);
  return internal::executeQuery(
    ctx,
    operation
  );
}

} // cgql
