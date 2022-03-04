#pragma once

#include "cgql/base/cgqlPch.h"

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

template<typename T>
class cgqlWeakPtr {
public:
  using data_type = T;
  using pointer_type = T*;
public:
  cgqlWeakPtr() = default;
  cgqlWeakPtr(const cgqlSPtr<data_type>& sharedPtr) {
    this->ptr = sharedPtr.get();
  }
  pointer_type get() const {
    return this->ptr;
  }
  data_type operator*() const {
    return *ptr;
  }
  pointer_type operator->() const {
    return ptr;
  }
private:
  pointer_type ptr;
};

namespace internal {
  enum GRAPHQL_INT_LIMITS {
    MAX = 2147483647,
    MIN = -2147483648
  };
}

} // cgql
