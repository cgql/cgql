#pragma once

#include "cgql/base/cgqlPch.h"

namespace cgql {
namespace internal {

constexpr inline bool isLetter(uint32_t value) {
  if(
    (value >= 0x0041 && value <= 0x005A) ||
    (value >= 0x0061 && value <= 0x007A)
  ) {
    return true;
  }
  return false;
}

constexpr inline bool isDigit(uint32_t value) {
  if(value >= 0x0030 && value <= 0x0039) {
    return true;
  }
  return false;
}

constexpr inline bool isAsciiDigit(uint8_t value) {
  return value >= 0 && value <= 9;
}

constexpr inline bool isNameStart(uint32_t value) {
  return isLetter(value) || value == 0x005F;
}

constexpr inline bool isNameContinue(uint32_t value) {
  return isLetter(value) || isDigit(value) || value == 0x005F;
}

template<typename T>
constexpr inline bool isList(const T& type) {
  return type.index() == 3;
}

template<typename T>
constexpr inline bool isObject(const T& type) {
  return type.index() == 4;
}

template<typename T>
constexpr inline T charToInt(const char& ch) {
  return ch - 48;
}

template<typename T>
constexpr inline T strToInt(const std::string& str) {
  T tempQuotient = 0;
  for(const char& ch : str) {
    tempQuotient = tempQuotient * 10 + charToInt<uint8_t>(ch);
  }
  return tempQuotient;
}

} // internal
} // cgql
