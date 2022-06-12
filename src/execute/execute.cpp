#include "cgql/execute/execute.h"
#include "selectionSetExecutor.h"

#include <cassert>

namespace cgql {

static ExecutionResult executeQuery(
  ExecutionContext& ctx,
  const OperationDefinition& query
) {
  const SelectionSet& selection = query.getSelectionSet();

  SelectionSetExecutor executor(ctx.schema->getQuery());
  return ExecutionResult {
    .data = executor.execute(ctx, selection),
    .errors = ctx.errorManager.getAllErrors()
  };
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

ExecutionResult execute(
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

  ExecutionContext ctx {
    .schema = schema,
    .resolverMap = resolverMap,
    .typeOfMap = typeOfMap,
    .fragments = fragments
  };
  return executeQuery(
    ctx,
    operation
  );
}

} // cgql
