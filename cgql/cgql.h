#ifndef CGQL_H
#define CGQL_H

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/type/Document.h"

namespace cgql {

class CgqlInstance {
public:
  CgqlInstance();
  ~CgqlInstance();
  void useSchema(const cgqlSPtr<internal::Schema>& schema);
  cgqlSPtr<ResultMap> executeWith(const internal::Document& document, const ResolverMap& resolverMap);
private:
  cgqlSPtr<internal::Schema> schema;
};

} // end of cgql

#endif /* end of include guard: CGQL_H */
