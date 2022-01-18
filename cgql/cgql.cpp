#include "cgql/cgql.h"
#include "cgql/execute/execute.h"
#include "cgql/schema/GraphQLTypes.h"

namespace cgql {

CgqlInstance::CgqlInstance() {}
CgqlInstance::~CgqlInstance() {}
void CgqlInstance::useSchema(const cgqlSPtr<internal::Schema>& schema) {
  this->schema = schema;
}
cgqlSPtr<ResultMap> CgqlInstance::executeWith(const internal::Document& document, const ResolverMap& resolverMap) {
  return execute(this->schema, document, resolverMap);
}

} // end of cgql
