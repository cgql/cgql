#include "../../cgqlPch.h"

namespace cgql {
namespace internal {

using std::string;

inline bool isLetter(uint32_t value) {
  if(
    (value >= 0x0041 && value <= 0x005A) ||
    (value >= 0x0061 && value <= 0x007A)
  ) {
    return true;
  }
  return false;
}

inline bool isDigit(uint32_t value) {
  if(value >= 0x0030 && value <= 0x0039) {
    return true;
  }
  return false;
}

inline bool isNameStart(uint32_t value) {
  return isLetter(value) || value == 0x005F;
}

inline bool isNameContinue(uint32_t value) {
  return isLetter(value) || isDigit(value) || value == 0x005F;
}

} // internal
} // cgql
