#pragma once

#include <exception>
#include <sstream>
#include <string>

namespace cgql {

class NonNullValueException : public std::exception {
public:
  NonNullValueException(const std::string& fieldName) {
    std::stringstream stream;
    stream << "Expected non-null value for field: ";
    stream << fieldName;
    message = stream.str();
  }
  char const* what() const noexcept override { return this->message.c_str(); };
private:
  std::string message = "Value is null";
};

}
