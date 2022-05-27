#pragma once

#include <iostream>

namespace cgql {

class logger {
public:
  template<typename T>
  static void print(const char *ansiCode, T msg) {
    std::cout << ansiCode << msg << "\u001b[0m" << std::endl;
  }
  template<typename T>
  static void info(T msg) {
    print<T>("\u001b[38;5;226m", msg);
  }
  template<typename T>
  static void success(T msg) {
    print<T>("\u001b[38;5;48m", msg);
  }
  template<typename T>
  static void error(T msg) {
    print<T>("\u001b[38;5;196m", msg);
  }
}; // logger

} // cgql
