#include "logger.h"

namespace logger {
  void print(const char *ansiCode, const char *msg) {
    std::cout << ansiCode << msg << RESET;
  }

  void info(const char *msg) {
    print(INFO, msg);
  }
  void success(const char *msg) {
    print(SUCCESS, msg);
  }
  void error(const char *msg) {
    print(ERROR, msg);
  }
};
