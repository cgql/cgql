#ifndef ABSTRACT_TYPE_H
#define ABSTRACT_TYPE_H

#include "../cgqlPch.h"

namespace cgql {
namespace internal {

using std::string;

class AbstractTypeDefinition {
public:
  inline void setName(const string& name) {
    this->name = name;
  };
  inline const string& getName() const {
    return this->name;
  };
protected:
  string name;
};

} // internal
} // cgql

#endif /* ifndef ABSTRACT_TYPE_H */
