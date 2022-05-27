#include "selectionSetExecutor.h"

namespace cgql {

static void mergeSelectionSet(
  const SelectionSet& fields,
  SelectionSet& mergedSelectionSet
) {
  for(const cgqlSPtr<Selection>& field : fields) {
    const SelectionSet& fieldSelectionSet = field->getSelectionSet();
    if(fieldSelectionSet.empty()) continue;
    mergedSelectionSet.insert(
      mergedSelectionSet.end(),
      fieldSelectionSet.begin(),
      fieldSelectionSet.end()
    );
  }
}

static Data defaultFieldResolver(
  cgqlSPtr<Object> source,
  const std::string& name
) {
  auto it = source->fields.find(name);
  if(it == source->fields.end()) {
    return std::monostate{};
  }
  return it->second;
}

Data SelectionSetExecutor::completeList(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
  const SelectionSet& fields,
  const Data& result
) {
  cgqlSPtr<List> rawResultList =
    fromVariant<cgqlSPtr<List>>(result);
  cgqlSPtr<List> resultList = cgqlSMakePtr<List>();
  resultList->elements.reserve(rawResultList->elements.size());
  for(Data rawResult : rawResultList->elements) {
    resultList->elements.push_back(
      completeValue(
        ctx,
        field,
        fieldType->getInnerType(),
        fields,
        rawResult
      )
    );
  }
  return resultList;
}

Data SelectionSetExecutor::completeAbstractType(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const Data& result
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

      SelectionSet selectionSet;
      mergeSelectionSet(fields, selectionSet);

      SelectionSetExecutor executor(object, resultMap);
      return executor.execute(ctx, selectionSet);
    }
  }
  assert(false && "Unable to resolve value for implementation of interface");
  /* silence compiler warning */ return 0;
}

Args SelectionSetExecutor::buildArgumentMap(
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
    cgqlContainer<Argument>::const_iterator it = std::find_if(
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
      assert(false && "Value is null or not provided");
    } else {
      arg.argsMap.try_emplace(
        argName,
        it->getValue()
      );
    }
  }
  return arg;
}

cgqlSPtr<Object> SelectionSetExecutor::execute(
  const ExecutionContext& ctx,
  const SelectionSet& selectionSet
) {
  GroupedField groupedFieldSet;
  collectFields(ctx, obj, selectionSet, groupedFieldSet);

  cgqlSPtr<Object> resultObj = cgqlSMakePtr<Object>();
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    FieldTypeDefinition field = findGraphQLFieldByName(
      obj,
      std::static_pointer_cast<Field>(fields.front())->getName()
    );
    resultObj->fields.try_emplace(
      responseKey,
      executeField(
        ctx,
        field,
        field.getFieldType(),
        fields
      )
    );
  }

  return resultObj;
}

Data SelectionSetExecutor::executeField(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields
) {
  ResolverMap::const_iterator it = ctx.resolverMap.find(field.getName());
  Data result = [&]() {
    if(it != ctx.resolverMap.end()) {
      return it->second(buildArgumentMap(
        fields.front(),
        field
      ));
    } else {
      return defaultFieldResolver(
        source,
        field.getName()
      );
    }
  }();
  return completeValue(
    ctx,
    field,
    fieldType,
    fields,
    result
  );
}

Data SelectionSetExecutor::completeValue(
  const ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const Data& result
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
      field,
      nonNull->getInnerType(),
      fields,
      result
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
        field,
        defaultWrap->getInnerType(),
        fields,
        result
      );
    }
    case DefinitionType::LIST: {
      cgqlSPtr<ListTypeDefinition<TypeDefinition>> list =
        std::static_pointer_cast<ListTypeDefinition<TypeDefinition>>(fieldType);
      return completeList(
        ctx,
        field,
        list,
        fields,
        result
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

      cgqlSPtr<Object> objectValue = fromVariant<cgqlSPtr<Object>>(result);

      SelectionSet selectionSet;
      mergeSelectionSet(fields, selectionSet);

      SelectionSetExecutor executor(schemaObj, objectValue);
      return executor.execute(ctx, selectionSet);
    }
    case DefinitionType::INTERFACE: {
      cgqlSPtr<InterfaceTypeDefinition> schemaObj =
        std::static_pointer_cast<InterfaceTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result);
    }
    case DefinitionType::UNION: {
      cgqlSPtr<UnionTypeDefinition> schemaObj =
        std::static_pointer_cast<UnionTypeDefinition>(fieldType);
      return completeAbstractType(ctx, schemaObj, fields, result);
    }
    case DefinitionType::ENUM: {
      cgqlSPtr<EnumTypeDefinition> enumType =
        std::static_pointer_cast<EnumTypeDefinition>(fieldType);
      return enumType->serialize(fromVariant<String>(result));
    }
    default:
      assert(false && "TypeDef cannot be base");
  }
  /* silence compiler warning */ return 0;
}

}
