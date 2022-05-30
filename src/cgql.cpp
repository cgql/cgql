#include "cgql/cgql.h"
#include "cgql/execute/execute.h"
#include "cgql/schema/GraphQLTypes.h"
#include "cgql/schema/typeRegistry.h"
#include "cgql/parser/schemaParser.h"

namespace cgql {

cgqlSPtr<Schema> CgqlInstance::parseSchema(const char *schema) {
  this->typeRegistry.init();
  SchemaParser parser(schema);
  parser.parse(this->typeRegistry);
  this->schema = cgqlSMakePtr<Schema>();
  this->schema->setTypeDefMap(this->typeRegistry.getAllTypes());
  return this->schema;
}

cgqlSPtr<Object> CgqlInstance::executeWith(
  const Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
) {
  return execute(this->schema, document, resolverMap, typeOfMap);
}

} // end of cgql
