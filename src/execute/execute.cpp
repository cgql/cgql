#include "cgql/execute/execute.h"
#include "selectionSetExecutor.h"
#include "cgql/logger/logger.h"

#include <cassert>

namespace cgql {

static cgqlSPtr<Object> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query
) {
  const SelectionSet& selection = query.getSelectionSet();

  SelectionSetExecutor executor(ctx.schema->getQuery());
  return executor.execute(ctx, selection);
}

struct OperationInfo {
  OperationDefinition operation;
  cgqlContainer<FragmentDefinition> fragments;
};

static OperationInfo getOperation(
  const Document& document,
  OperationType operationName
) {
  OperationInfo operationInfo;
  for(const Definition& def : document.getDefinitions()) {
    if(def.index() == 0) {
      OperationDefinition opDef =
        fromVariant<OperationDefinition>(def);
      if(opDef.getOperationType() == operationName) {
        operationInfo.operation = opDef;
      }
    } else if(def.index() == 1) {
      FragmentDefinition fragmentDef =
        fromVariant<FragmentDefinition>(def);
      operationInfo.fragments.push_back(fragmentDef);
    }
  }
  return operationInfo;
}

cgqlSPtr<Object> execute(
  const cgqlSPtr<Schema> &schema,
  const Document &document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  // get operation

  auto [
    operation,
    fragments
  ] = getOperation(document, OperationType::QUERY);

  ExecutionContext ctx;
  ctx.schema = schema;
  ctx.resolverMap = resolverMap;
  ctx.typeOfMap = typeOfMap;
  ctx.fragments = fragments;
  return executeQuery(
    ctx,
    operation
  );
}

} // cgql
