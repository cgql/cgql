#include "cgql/schema/typeRegistry.h"

#include <cassert>

namespace cgql {

static inline cgqlSPtr<ScalarTypeDefinition> MAKE_SCALAR(
  std::string name,
  Serializer serializer
) {
  return cgqlSMakePtr<ScalarTypeDefinition>(name, serializer);
}

static inline bool isInteger(const Data& value) {
  return value.index() == 0;
}

static inline bool isString(const Data& value) {
  return value.index() == 1;
}

void TypeRegistry::addScalar(cgqlSPtr<ScalarTypeDefinition> type) const {
  this->types.try_emplace(type->getName(), type);
};

cgqlSPtr<TypeDefinition> TypeRegistry::getType(std::string typeName) const {
  return types[typeName];
}

void TypeRegistry::init() {
  this->addScalar(MAKE_SCALAR(
    "Int",
    [](const Data& value) -> const Data& {
      if(!isInteger(value)) {
        assert(false && "Value is not Integer");
      }
      return value;
    }
  ));
  this->addScalar(MAKE_SCALAR(
    "String",
    [](const Data& value) -> const Data& {
      if(!isString(value)) {
        assert(false && "Value is not String");
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

} // end of cgql
