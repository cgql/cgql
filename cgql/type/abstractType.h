#ifndef ABSTRACT_TYPE_H
#define ABSTRACT_TYPE_H

#include "cgql/cgqlPch.h"

namespace cgql {
namespace internal {

class AbstractTypeDefinition {
public:
  inline void setName(const std::string& name) {
    this->name = name;
  };
  constexpr inline const std::string& getName() const {
    return this->name;
  };
protected:
  std::string name;
};

} // internal
} // cgql

#endif /* ifndef ABSTRACT_TYPE_H */
