#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

#define MAKE_SCALAR(name, serializer) cgqlSMakePtr<ScalarTypeDefinition>(name, serializer)

inline bool isInteger(const Data& value) {
  return value.index() == 0;
}

inline bool isString(const Data& value) {
  return value.index() == 1;
}

void TypeRegistry::init() {
  this->addScalar(MAKE_SCALAR(
    "Int",
    [](const Data& value) -> const Data& {
      if(!isInteger(value)) {
        cgqlAssert(false, "Value is not Integer");
      }
      return value;
    }
  ));
  this->addScalar(MAKE_SCALAR(
    "String",
    [](const Data& value) -> const Data& {
      if(!isString(value)) {
        cgqlAssert(false, "Value is not String");
      }
      return value;
    }
  ));
  this->addScalar(MAKE_SCALAR(
    "Float",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
  this->addScalar(MAKE_SCALAR(
    "ID",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
  this->addScalar(MAKE_SCALAR(
    "Boolean",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
}

} // end of internal
} // end of cgql
