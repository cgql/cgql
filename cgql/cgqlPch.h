#ifndef CGQL_PCH
#define CGQL_PCH

#include <iostream>

#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>

#include <memory>
#include <functional>
#include <string>

#include <exception>

#include <type_traits>

namespace cgql {
namespace internal {

template<typename T>
using cgqlSPtr = std::shared_ptr<T>;

#define cgqlSMakePtr std::make_shared

} // internal
} // cgql

#endif
