#ifndef CGQL_H
#define CGQL_H

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/query/Document.h"
#include "cgql/schema/typeRegistry.h"

namespace cgql {

class CgqlInstance {
public:
  CgqlInstance();
  void useSchema(const cgqlSPtr<internal::Schema>& schema);
  cgqlSPtr<Object> executeWith(
    const internal::Document& document,
    const ResolverMap& resolverMap,
    const TypeOfMap& typeOfMap
  );
  cgqlSPtr<internal::Schema> parseSchema(const char* schema);
private:
  cgqlSPtr<internal::Schema> schema;
  internal::TypeRegistry typeRegistry;
};

} // end of cgql

#endif /* end of include guard: CGQL_H */
