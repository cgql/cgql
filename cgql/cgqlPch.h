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

#include <algorithm>
#include <type_traits>

namespace cgql {

template<typename T>
using cgqlSPtr = std::shared_ptr<T>;

#define cgqlSMakePtr std::make_shared

template<typename T>
using cgqlContainer = std::vector<T>;

} // cgql

#endif
