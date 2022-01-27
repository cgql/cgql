#include "cgql/cgql.h"
#include "cgql/execute/execute.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/base/typeRegistry.h"

namespace cgql {

CgqlInstance::CgqlInstance() {}
CgqlInstance::~CgqlInstance() {}

void CgqlInstance::init() {
}

void CgqlInstance::useSchema(const cgqlSPtr<internal::Schema>& schema) {
  this->schema = schema;
}
cgqlUPtr<ResultMap> CgqlInstance::executeWith(
  const internal::Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  return execute(this->schema, document, resolverMap, typeOfMap);
}

} // end of cgql
