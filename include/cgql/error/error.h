#pragma once

#include "cgql/cgqlDefs.h"

#include <string>

namespace cgql {

struct Error {
  Error(std::string message) : message(message) {}
  std::string message;
};

class ErrorManager {
public:
  void addError(const char* errorMsg) {
    this->errors.emplace_back(errorMsg);
  }
  const auto& getAllErrors() const {
    return this->errors;
  }
private:
  cgqlContainer<Error> errors;
};

}
