#ifndef LOGGER_H
#define LOGGER_H

#include "cgql/base/cgqlPch.h"

namespace cgql {

class logger {
public:
  template<typename T>
  inline static void print(const char *ansiCode, T msg) {
    std::cout << ansiCode << msg << RESET << std::endl;
  }
  template<typename T>
  inline static void info(T msg) {
    print<T>(INFO, msg);
  }
  template<typename T>
  inline static void success(T msg) {
    print<T>(SUCCESS, msg);
  }
  template<typename T>
  inline static void error(T msg) {
    print<T>(ERROR, msg);
  }
  inline static const char* INFO = "\u001b[38;5;226m";
  inline static const char* SUCCESS = "\u001b[38;5;48m";
  inline static const char* ERROR = "\u001b[38;5;196m";
  inline static const char* RESET = "\u001b[0m";
}; // logger

} // cgql

#endif
