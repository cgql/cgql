#include "coerceArgumentValues.h"
#include "cgql/logger/logger.h"

namespace cgql {

GraphQLInputTypes coerceArgumentValues(
  ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& type,
  const GraphQLInputTypes& value
) {
  switch(type->getDefinitionType()) {
    case DefinitionType::DEFAULT_WRAP: {
      cgqlSPtr<DefaultWrapTypeDefinition> wrappedObj =
        std::static_pointer_cast<DefaultWrapTypeDefinition>(type);
      return coerceArgumentValues(ctx, wrappedObj->getInnerType(), value);
    }
    case DefinitionType::INPUT_OBJECT: {
      if(value.index() != 2) {
        ctx.errorManager.addError(Error{"expected an object as input type"});
        return std::monostate{};
      }

      cgqlSPtr<ObjectType> object = fromVariant<cgqlSPtr<ObjectType>>(value);
      cgqlSPtr<ObjectType> coercedObject = cgqlSMakePtr<ObjectType>();
      cgqlSPtr<InputObjectTypeDefinition> inputObject =
        std::static_pointer_cast<InputObjectTypeDefinition>(type);
      for(const auto& field : inputObject->getFields()) {
        auto it = object->fields.find(field.getName());

        coercedObject->fields.try_emplace(
          field.getName(),
          it != object->fields.end()
            ? coerceArgumentValues(ctx, field.getInputValueType(), it->second)
            : field.getDefaultValue()
        );
      }
      return coercedObject;
    }
    case DefinitionType::LIST: {
      if(value.index() != 3) {
        ctx.errorManager.addError(Error{"expected a list as input type"});
        return std::monostate{};
      }

      cgqlSPtr<ListType> list = fromVariant<cgqlSPtr<ListType>>(value);
      cgqlSPtr<ListType> coercedList = cgqlSMakePtr<ListType>();
      coercedList->elements.reserve(list->elements.size());
      cgqlSPtr<ListTypeDefinition> listType =
        std::static_pointer_cast<ListTypeDefinition>(type);

      for(const auto& element : list->elements) {
        coercedList->elements.push_back(
          coerceArgumentValues(ctx , listType->getInnerType(), element)
        );
      }
      return coercedList;
    }
    default: return value;
  }
}

}
