#pragma once

#include "cgql/logger/logger.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace cgql {

#define cgqlAssert(expression, message) \
  assert(expression);
} // cgql
