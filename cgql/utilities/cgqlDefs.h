#ifndef CGQL_DEFS_H
#define CGQL_DEFS_H

#include "cgql/cgqlPch.h"

namespace cgql {

template<typename T>
using cgqlSPtr = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr cgqlSPtr<T> cgqlSMakePtr(Args&& ... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using cgqlContainer = std::vector<T>;

} // cgql

#endif /* ifndef CGQL_DEFS_H */
