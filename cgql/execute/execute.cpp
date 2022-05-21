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
  for(const FieldTypeDefinition& field : objectType->getFields()) {
    if(fieldName == field.getName()) {
      return field;
    }
  }
  std::string msg;
  msg += "Field with name ";
  msg += fieldName;
  msg += " cannot be found in object ";
  msg += objectType->getName();
  cgqlAssert(false, msg.c_str());
  /* silence compiler warning */ return objectType->getFields()[0];
}

void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedFields
) {
  for(const cgqlSPtr<Selection>& selection : selectionSet) {
    const SelectionType type = selection->getSelectionType();
    switch(type) {
      case SelectionType::BASE:
        cgqlAssert(false, "Invalid selection type in execution: BASE");
      case SelectionType::FIELD: {
        cgqlSPtr<Field> field =
          std::static_pointer_cast<Field>(selection);

        const std::string& responseKey = field->getResponseKey();

        groupedFields[responseKey].emplace_back(field);
      }
      case SelectionType::INLINE_FRAGMENT: {
        cgqlSPtr<InlineFragment> inlineFragment =
          std::static_pointer_cast<InlineFragment>(selection);
        const std::string& typeCondition = inlineFragment->getTypeCondition();
        
        if(typeCondition != objectType->getName()) continue;

        collectFields(
          ctx,
          objectType,
          inlineFragment->getSelectionSet(),
          groupedFields
        );
      }
      case SelectionType::FRAGMENT: {
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
}

template<typename T>
void collectSubFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& objectType,
  const SelectionSet& selectionSet,
  GroupedField& groupedFields
) {
  for(const cgqlSPtr<Selection>& selection : selectionSet) {
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
  for(const cgqlSPtr<Selection>& field : fields) {
    const SelectionSet& fieldSelectionSet = field->getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    for(const cgqlSPtr<Selection>& subField : fieldSelectionSet) {
      mergedSelectionSet.emplace_back(subField);
    }
  }
}

Data completeList(
  const ExecutionContext& ctx,
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<Object>>& source
) {
  cgqlSPtr<List> rawResultList =
    fromVariant<cgqlSPtr<List>>(result);
  cgqlSPtr<List> resultList = cgqlSMakePtr<List>();
  resultList->elements.reserve(rawResultList->elements.size());
  for(const Data& rawResult : rawResultList->elements) {
    resultList->elements.emplace_back(
      completeValue(
        ctx,
        fieldType->getInnerType(),
        field,
        fields,
        rawResult,
        source
      )
    );
  }
  return resultList;
}

static cgqlUPtr<Object> executeGroupedFieldSet(
  const ExecutionContext& ctx,
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const GroupedField& groupedFieldSet,
  const std::optional<cgqlSPtr<Object>>& source
) {
  cgqlUPtr<Object> resultMap = cgqlUMakePtr<Object>();
  resultMap->fields.reserve(groupedFieldSet.size());

  for(auto const& [responseKey, fields] : groupedFieldSet) {
    FieldTypeDefinition field = findGraphQLFieldByName(
      objectType,
      std::static_pointer_cast<Field>(fields[0])->getName()
    );
    resultMap->fields.try_emplace(
      responseKey,
      executeField(
        ctx,
        field,
        field.getFieldType(),
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
  cgqlSPtr<Object> v =
    fromVariant<cgqlSPtr<Object>>(result);
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
  const std::optional<cgqlSPtr<Object>>& source
) {
  cgqlSPtr<Object> resultMap =
    fromVariant<cgqlSPtr<Object>>(result);
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& possibleTypes =
    ctx.schema->getPossibleTypes(fieldType);
  TypeOfMap::const_iterator it = ctx.typeOfMap.find(fieldType->getName());
  for(const cgqlSPtr<TypeDefinition>& possibleType : possibleTypes) {
    String typeName = it->second(resultMap);
    if(possibleType->getName() == typeName) {
      cgqlSPtr<ObjectTypeDefinition> object =
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
  cgqlAssert(false, "Unable to resolve value for implementation of interface");
  /* silence compiler warning */ return 0;
}

template<typename T>
Data completeValue(
  const ExecutionContext& ctx,
  const cgqlSPtr<T>& fieldType,
  const FieldTypeDefinition& field,
  const SelectionSet& fields,
  const Data& result,
  const std::optional<cgqlSPtr<Object>>& source
) {
  const DefinitionType type = fieldType->getDefinitionType();
  if(type == DefinitionType::NON_NULL) {
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
  } else if(result.index() == 4) {
    return std::monostate{};
  }
  switch (type) {
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
    case DefinitionType::LIST: {
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
    case DefinitionType::SCALAR: {
      cgqlSPtr<ScalarTypeDefinition> scalar =
        std::static_pointer_cast<ScalarTypeDefinition>(fieldType);
      return scalar->serializer(result);
    }
    case DefinitionType::OBJECT: {
      cgqlSPtr<ObjectTypeDefinition> schemaObj =
        std::static_pointer_cast<ObjectTypeDefinition>(fieldType);

      return executeInnerSelectionSet<ObjectTypeDefinition>(
        ctx,
        schemaObj,
        result,
        fields
      );
    }
    case DefinitionType::INTERFACE: {
      cgqlSPtr<InterfaceTypeDefinition> schemaObj =
        std::static_pointer_cast<InterfaceTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result, source);
    }
    case DefinitionType::UNION: {
      cgqlSPtr<UnionTypeDefinition> schemaObj =
        std::static_pointer_cast<UnionTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result, source);
    }
    case DefinitionType::ENUM: {
      cgqlSPtr<EnumTypeDefinition> enumType =
        std::static_pointer_cast<EnumTypeDefinition>(fieldType);
      return enumType->serialize(fromVariant<String>(result));
    }
    default:
      cgqlAssert(false, "TypeDef cannot be base");
  }
  /* silence compiler warning */ return 0;
}

Args buildArgumentMap(
  const cgqlSPtr<Selection>& selection,
  const FieldTypeDefinition& fieldType
) {
  Args arg;
  cgqlSPtr<Field> field =
    std::static_pointer_cast<Field>(selection);
  const cgqlContainer<Argument>& argumentValues =
    field->getArgs();
  const cgqlContainer<InputValueDefinition>& argumentDefinitions =
    fieldType.getArgs();
  for(const InputValueDefinition& argDef : argumentDefinitions) {
    const std::string& argName = argDef.getName();
    const auto& it = std::find_if(
      argumentValues.begin(),
      argumentValues.end(),
      [&argName](const Argument& arg) {
        return arg.getName() == argName;
      }
    );
    const GraphQLInputTypes defaultValue = argDef.getDefaultValue();
    bool hasValue = it != argumentValues.end();
    if(!hasValue) {
      arg.argsMap.try_emplace(
        argName,
        defaultValue
      );
    } else if (
      argDef.getInputValueType()->getDefinitionType() == DefinitionType::NON_NULL &&
      !hasValue
    ) {
      cgqlAssert(false, "Value is null or not provided");
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
  const std::optional<cgqlSPtr<Object>>& source
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

cgqlUPtr<Object> executeSelectionSet(
  const ExecutionContext& ctx,
  const SelectionSet &selectionSet,
  const cgqlSPtr<ObjectTypeDefinition> &obj,
  const std::optional<cgqlSPtr<Object>>& source
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

cgqlUPtr<Object> executeQuery(
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
  for(const Definition& def : document.getDefinitions()) {
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
  for(const Definition& def : document.getDefinitions()) {
    if(def.index() == 1) {
      fragments.emplace_back(fromVariant<FragmentDefinition>(def));
    }
  }
  return fragments;
}

} // internal

cgqlUPtr<Object> execute(
  const cgqlSPtr<internal::Schema> &schema,
  const internal::Document &document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  // get operation
  using namespace cgql::internal;
  ExecutionContext ctx;
  ctx.schema = schema;
  ctx.resolverMap = resolverMap;
  ctx.typeOfMap = typeOfMap;
  ctx.fragments = getFragmentsFromQuery(document);
  return executeQuery(
    ctx,
    getOperation(document)
  );
}

} // cgql
