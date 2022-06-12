#include "selectionSetExecutor.h"

#include "cgql/error/error.h"

namespace cgql {

using GroupedField = std::map<
  std::string,
  SelectionSet
>;

static void collectFields(
  ExecutionContext& ctx,
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
    }
  }
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
  ExecutionContext& ctx,
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
  ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const Data& result
) {
  cgqlSPtr<Object> resultMap =
    fromVariant<cgqlSPtr<Object>>(result);
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& possibleTypes =
    ctx.schema->getPossibleTypes(fieldType);

  TypeOfMap::const_iterator it = ctx.typeOfMap.find(fieldType->getName());
  String typeName = it->second(resultMap);
  
  if(typeName == "") {
    ctx.errorManager.addError(Error{
      "abstract type must resolve to runtime type "
      "which requires type name"
    });
    return std::monostate{};
  }

  for(const cgqlSPtr<TypeDefinition>& possibleType : possibleTypes) {
    if(possibleType->getName() != typeName) continue;

    cgqlSPtr<ObjectTypeDefinition> object =
      std::static_pointer_cast<ObjectTypeDefinition>(possibleType);

    SelectionSet selectionSet;
    mergeSelectionSet(fields, selectionSet);

    SelectionSetExecutor executor(object, resultMap);
    return executor.execute(ctx, selectionSet);
  }

  ctx.errorManager.addError(Error{
    "resolved type does not match any of the provided "
    "types in the abstract type"
  });
  return std::monostate{};
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
  ExecutionContext& ctx,
  const SelectionSet& selectionSet
) {
  GroupedField groupedFieldSet;
  collectFields(ctx, obj, selectionSet, groupedFieldSet);

  cgqlSPtr<Object> resultObj = cgqlSMakePtr<Object>();

  auto findField = [this](const std::string& name) {
    cgqlContainer<FieldTypeDefinition>::const_iterator it =
      std::find_if(
        obj->getFields().begin(),
        obj->getFields().end(),
        [&name](const FieldTypeDefinition& field) {
          return name == field.getName();
        }
      );
    return it;
  };

  for(auto const& [responseKey, fields] : groupedFieldSet) {
    cgqlSPtr<Field> fieldNode =
      std::static_pointer_cast<Field>(fields.front());
    auto fieldIter = findField(fieldNode->getName());
    if(fieldIter == obj->getFields().end()) continue;

    FieldTypeDefinition field = *fieldIter;
    Data executedFieldResult = executeField(
      ctx,
      field,
      field.getFieldType(),
      fields
    );
    resultObj->fields.try_emplace(
      responseKey,
      executedFieldResult
    );
  }

  return resultObj;
}

Data SelectionSetExecutor::executeField(
  ExecutionContext& ctx,
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
  ExecutionContext& ctx,
  const FieldTypeDefinition& field,
  const cgqlSPtr<TypeDefinition>& fieldType,
  const SelectionSet& fields,
  const Data& result
) {
  const DefinitionType type = fieldType->getDefinitionType();
  if(type == DefinitionType::NON_NULL) {
    cgqlSPtr<NonNullTypeDefinition> nonNull =
      std::static_pointer_cast<NonNullTypeDefinition>(fieldType);
    Data completedValue = completeValue(
      ctx,
      field,
      nonNull->getInnerType(),
      fields,
      result
    );
    if(result.index() == 4) {
      ctx.errorManager.addError(Error{"expected a non-null value"});
      return std::monostate{};
    }
    return completedValue;
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
      String enumValue = fromVariant<String>(result);
      return enumType->serialize(enumValue);
    }
    default:
      assert(false && "TypeDef cannot be base");
      return std::monostate{};
  }
}

}
