#include <iostream>
namespace logger {
  #define INFO "\u001b[38;5;226m"
  #define SUCCESS "\u001b[38;5;48m"
  #define ERROR "\u001b[38;5;196m"
  #define RESET "\u001b[0m"

  template<typename T>
  inline void print(const char *ansiCode, T msg) {
    std::cout << ansiCode << msg << RESET << std::endl;
  }

  template<typename T>
  inline void info(T msg) {
    print<T>(INFO, msg);
  }
  template<typename T>
  inline void success(T msg) {
    print<T>(SUCCESS, msg);
  }
  template<typename T>
  inline void error(T msg) {
    print<T>(ERROR, msg);
  }
};
