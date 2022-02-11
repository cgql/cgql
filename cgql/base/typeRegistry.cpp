#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

#define MAKE_SCALAR(name, DataType) cgqlSMakePtr<ScalarTypeDefinition<DataType>>(name)

void TypeRegistry::init() {
  this->addType(MAKE_SCALAR("Int", Int));
  this->addType(MAKE_SCALAR("String", String));
}

} // end of internal
} // end of cgql
