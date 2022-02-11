#include "cgql/cgql.h"
#include "cgql/execute/execute.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/base/typeRegistry.h"
#include "cgql/type/parser/parser.h"
#include "cgql/schema/parser.h"

namespace cgql {

CgqlInstance::CgqlInstance() {
  this->typeRegistry.init();
}

void CgqlInstance::useSchema(const cgqlSPtr<internal::Schema>& schema) {
  this->schema = schema;
}

cgqlSPtr<internal::Schema> CgqlInstance::parseSchema(const char *schema) {
  auto parsedSchema = internal::parseSchema(schema, this->typeRegistry);
  this->useSchema(parsedSchema);
  return parsedSchema;
}

cgqlUPtr<ResultMap> CgqlInstance::executeWith(
  const internal::Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  return execute(this->schema, document, resolverMap, typeOfMap);
}

} // end of cgql
