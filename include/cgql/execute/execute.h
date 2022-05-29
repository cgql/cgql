#pragma once

#include "cgql/schema/typeDefinitions.h"
#include "cgql/query/Document.h"

#include "cgql/cgqlDefs.h"

namespace cgql {

cgqlSPtr<Object> execute(
  const cgqlSPtr<Schema>& schema,
  const Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
