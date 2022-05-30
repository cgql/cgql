#include "selectionSetExecutor.h"

namespace cgql {

using GroupedField = std::map<
  std::string,
  SelectionSet
>;

static void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedFields
) {
  for(const cgqlSPtr<Selection>& selection : selectionSet) {
    const SelectionType type = selection->getSelectionType();
    switch(type) {
      case SelectionType::FIELD: {
        cgqlSPtr<Field> field =
          std::static_pointer_cast<Field>(selection);

        const std::string& responseKey = field->getResponseKey();

        groupedFields[responseKey].push_back(field);
        break;
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
        break;
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
        break;
      }
      case SelectionType::BASE:
        assert(false && "Invalid selection type in execution: BASE");
        break;
    }
  }
}

static FieldTypeDefinition findGraphQLFieldByName(
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const std::string& fieldName
) {
  cgqlContainer<FieldTypeDefinition>::const_iterator it =
    std::find_if(
      objectType->getFields().begin(),
      objectType->getFields().end(),
      [&fieldName](const FieldTypeDefinition& field) {
        return fieldName == field.getName();
      }
    );
  if(it == objectType->getFields().end()) {
    // TODO:
    return {};
  }
  return *it;
}

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
  const cgqlSPtr<ListTypeDefinition>& fieldType,
  const SelectionSet& fields,
  const Data& result
) {
  cgqlSPtr<List> rawResultList =
    fromVariant<cgqlSPtr<List>>(result);
  cgqlSPtr<List> resultList = cgqlSMakePtr<List>();
  resultList->elements.reserve(rawResultList->elements.size());
  for(Data rawResult : rawResultList->elements) {
    resultList->elements.emplace_back(
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
    if(possibleType->getName() != typeName) continue;

    cgqlSPtr<ObjectTypeDefinition> object =
      std::static_pointer_cast<ObjectTypeDefinition>(possibleType);

    SelectionSet selectionSet;
    mergeSelectionSet(fields, selectionSet);

    SelectionSetExecutor executor(object, resultMap);
    return executor.execute(ctx, selectionSet);
  }
  assert(false && "Unable to resolve value for implementation of interface");
  /* silence compiler warning */ return 0;
}

Args SelectionSetExecutor::buildArgumentMap(
  const cgqlSPtr<Selection>& selection,
  const FieldTypeDefinition& fieldType
) {
  cgqlSPtr<Field> field =
    std::static_pointer_cast<Field>(selection);
  const cgqlContainer<Argument>& argumentValues =
    field->getArgs();

  Args args;
  for(const InputValueDefinition& argDef : fieldType.getArgs()) {
    const std::string& argName = argDef.getName();
    cgqlContainer<Argument>::const_iterator it = std::find_if(
      argumentValues.begin(),
      argumentValues.end(),
      [&argName](const Argument& argument) {
        return argument.getName() == argName;
      }
    );
    const GraphQLInputTypes defaultValue = argDef.getDefaultValue();
    bool hasValue = it != argumentValues.end();
    if(!hasValue) {
      args.addArg(
        argName,
        defaultValue
      );
    } else if (
      argDef.getInputValueType()->getDefinitionType() == DefinitionType::NON_NULL &&
      !hasValue
    ) {
      assert(false && "Value is null or not provided");
    } else {
      args.addArg(
        argName,
        it->getValue()
      );
    }
  }
  return args;
}

cgqlSPtr<Object> SelectionSetExecutor::execute(
  const ExecutionContext& ctx,
  const SelectionSet& selectionSet
) {
  GroupedField groupedFieldSet;
  collectFields(ctx, obj, selectionSet, groupedFieldSet);

  cgqlSPtr<Object> resultObj = cgqlSMakePtr<Object>();
  for(auto const& [responseKey, fields] : groupedFieldSet) {
    cgqlSPtr<Field> fieldNode =
      std::static_pointer_cast<Field>(fields.front());
    FieldTypeDefinition field = findGraphQLFieldByName(
      obj,
      fieldNode->getName()
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
    cgqlSPtr<NonNullTypeDefinition> nonNull =
      std::static_pointer_cast<NonNullTypeDefinition>(fieldType);
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
      cgqlSPtr<DefaultWrapTypeDefinition> defaultWrap =
        std::static_pointer_cast<DefaultWrapTypeDefinition>(fieldType);
      return completeValue(
        ctx,
        field,
        defaultWrap->getInnerType(),
        fields,
        result
      );
    }
    case DefinitionType::LIST: {
      cgqlSPtr<ListTypeDefinition> list =
        std::static_pointer_cast<ListTypeDefinition>(fieldType);
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
