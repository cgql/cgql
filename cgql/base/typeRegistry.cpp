#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

#define MAKE_SCALAR(name) cgqlSMakePtr<ScalarTypeDefinition>(name)

void TypeRegistry::init() {
  this->addType(MAKE_SCALAR("Int"));
  this->addType(MAKE_SCALAR("String"));
}

} // end of internal
} // end of cgql
