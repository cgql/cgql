#include "cgql/cgql.h"
#include "cgql/execute/execute.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/schema/typeRegistry.h"
#include "cgql/parser/schemaParser.h"

namespace cgql {

CgqlInstance::CgqlInstance() {
  this->typeRegistry.init();
}

void CgqlInstance::useSchema(const cgqlSPtr<Schema>& schema) {
  this->schema = schema;
}

cgqlSPtr<Schema> CgqlInstance::parseSchema(const char *schema) {
  auto parsedSchema = parseSDLSchema(schema, this->typeRegistry);
  this->useSchema(parsedSchema);
  return parsedSchema;
}

cgqlSPtr<Object> CgqlInstance::executeWith(
  const Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  return execute(this->schema, document, resolverMap, typeOfMap);
}

} // end of cgql
