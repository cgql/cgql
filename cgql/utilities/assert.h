#ifndef CGQL_ASSERT
#define CGQL_ASSERT

#include "cgql/logger/logger.h"

#include <cstdio>
#include <cstdlib>

namespace cgql {

#ifdef NDEBUG
  #define cgqlAssert(expression, message) \
  internal::cgqlAssertImpl(               \
      #expression,                        \
      expression,                         \
      __FILE__,                           \
      __LINE__,                           \
      message                             \
    );
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
  if(!expression) {
    printf(
      "%sAssert failed%s: %s%s%s\n"
      "Expected: %s\n"
      "From file: %s\n"
      "At line: %d\n",
      ERROR,
      RESET,
      INFO,
      msg,
      RESET,
      expressionStr,
      filePath,
      line
    );
    abort();
  }
}

} // internal
} // cgql

#endif /* ifndef CGQL_ASSERT */
