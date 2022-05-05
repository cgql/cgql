#pragma once

#include "cgql/logger/logger.h"

#include <cstdio>
#include <cstdlib>

namespace cgql {

#if defined(NDEBUG)
  #define cgqlAssert(expression, message) \
  internal::cgqlAssertImpl(               \
      #expression,                        \
      expression,                         \
      __FILE__,                           \
      __LINE__,                           \
      message                             \
    )
#else
  #define cgqlAssert(expression, message)
#endif

namespace internal {

inline void cgqlAssertImpl(
  const char* expressionStr,
  bool expression,
  const char* filePath,
  int line,
  const char* msg = ""
) {
  if(expression) {
    printf(
      "%sAssert failed%s: %s%s%s\n"
      "Expected: %s\n"
      "From file: %s\n"
      "At line: %d\n",
      logger::ERROR,
      logger::RESET,
      logger::INFO,
      msg,
      logger::RESET,
      expressionStr,
      filePath,
      line
    );
    abort();
  }
}

} // internal
} // cgql
