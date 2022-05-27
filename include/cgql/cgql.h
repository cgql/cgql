#ifndef CGQL_H
#define CGQL_H

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/query/Document.h"
#include "cgql/schema/typeRegistry.h"

namespace cgql {

class CgqlInstance {
public:
  CgqlInstance();
  void useSchema(const cgqlSPtr<Schema>& schema);
  cgqlSPtr<Object> executeWith(
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
