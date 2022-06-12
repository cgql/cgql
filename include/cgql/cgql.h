#ifndef CGQL_H
#define CGQL_H

#include "cgql/execute/execute.h"
#include "cgql/schema/typeDefinitions.h"
#include "cgql/query/Document.h"
#include "cgql/schema/typeRegistry.h"

namespace cgql {

class CgqlInstance {
public:
  ExecutionResult executeWith(
    const Document& document,
    const ResolverMap& resolverMap,
    const TypeOfMap& typeOfMap
  );
  cgqlSPtr<Schema> parseSchema(const char* schema);
private:
  cgqlSPtr<Schema> schema;
  TypeRegistry typeRegistry;
};

} // end of cgql

#endif /* end of include guard: CGQL_H */
