#pragma once

#include "cgql/cgqlDefs.h"

#include <string>

namespace cgql {

struct Error {
  std::string message;
};

class ErrorManager {
public:
  void addError(Error error) {
    this->errors.push_back(error);
  }
  auto getAllErrors() const {
    return this->errors;
  }
private:
  cgqlContainer<Error> errors;
};

}
