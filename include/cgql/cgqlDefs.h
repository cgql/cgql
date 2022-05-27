#pragma once

#include <memory>
#include <vector>

namespace cgql {

template<typename T>
using cgqlSPtr = std::shared_ptr<T>;

template<typename T>
using cgqlUPtr = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr cgqlSPtr<T> cgqlSMakePtr(Args&& ... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename ... Args>
constexpr cgqlUPtr<T> cgqlUMakePtr(Args&& ... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using cgqlContainer = std::vector<T>;

namespace internal {
  enum GRAPHQL_INT_LIMITS {
    MAX = 2147483647,
    MIN = -2147483648
  };
}

} // cgql
