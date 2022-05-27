#include "cgql/execute/execute.h"
#include "selectionSetExecutor.h"
#include "cgql/logger/logger.h"

#include "cgql/utils.h"

#include <cassert>

namespace cgql {

FieldTypeDefinition findGraphQLFieldByName(
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
  if(it != objectType->getFields().end()) {
    return *it;
  }
  // TODO:
  return {};
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

cgqlSPtr<Object> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query
) {
  const SelectionSet& selection = query.getSelectionSet();

  SelectionSetExecutor executor(ctx.schema->getQuery());
  return executor.execute(ctx, selection);
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

cgqlContainer<FragmentDefinition> getFragmentsFromQuery(const Document& document) {
  cgqlContainer<FragmentDefinition> fragments;
  for(const Definition& def : document.getDefinitions()) {
    if(def.index() == 1) {
      fragments.push_back(fromVariant<FragmentDefinition>(def));
    }
  }
  return fragments;
}

cgqlSPtr<Object> execute(
  const cgqlSPtr<Schema> &schema,
  const Document &document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  // get operation
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
