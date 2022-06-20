#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/schema/typeDefinitions.h"
#include "execute/selectionSetExecutor.h"
namespace cgql {

GraphQLInputTypes coerceArgumentValues(
  ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& type,
  const GraphQLInputTypes& value
);

}
