#include "cgql/base/typeRegistry.h"

namespace cgql {
namespace internal {

#define MAKE_SCALAR(name, serializer) cgqlSMakePtr<ScalarTypeDefinition>(name, serializer)

static inline bool isInteger(const Data& value) {
  return value.index() == 0;
}

static inline bool isString(const Data& value) {
  return value.index() == 1;
}

void TypeRegistry::init() {
  this->addType(MAKE_SCALAR(
    "Int",
    [](const Data& value) -> const Data& {
      if(isInteger(value)) return value;
      cgqlAssert(false, "Value is not Integer");
      return value;
    }
  ));
  this->addType(MAKE_SCALAR(
    "String",
    [](const Data& value) -> const Data& {
      if(isString(value)) return value;
      cgqlAssert(false, "Value is not String");
      return value;
    }
  ));
  this->addType(MAKE_SCALAR(
    "Float",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
  this->addType(MAKE_SCALAR(
    "ID",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
  this->addType(MAKE_SCALAR(
    "Boolean",
    [](const Data& value) -> const Data& {
      // yet to be implemented
      return value;
    }
  ));
}

} // end of internal
} // end of cgql
