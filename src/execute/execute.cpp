#include "cgql/execute/execute.h"
#include "selectionSetExecutor.h"
#include "cgql/logger/logger.h"

#include "cgql/utils.h"

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

static const OperationDefinition& getOperation(
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

static cgqlContainer<FragmentDefinition> getFragmentsFromQuery(const Document& document) {
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
    getOperation(document, OperationType::QUERY)
  );
}

} // cgql
