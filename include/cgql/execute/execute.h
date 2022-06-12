#pragma once

#include "cgql/schema/typeDefinitions.h"
#include "cgql/query/Document.h"

#include "cgql/cgqlDefs.h"

namespace cgql {

struct ExecutionResult {
  cgqlSPtr<Object> data;
  cgqlContainer<Error> errors;
};

ExecutionResult execute(
  const cgqlSPtr<Schema>& schema,
  const Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
