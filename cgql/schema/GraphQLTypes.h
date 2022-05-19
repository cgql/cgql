#pragma once

#include "cgql/base/cgqlPch.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/utilities/assert.h"

namespace cgql {

using Int = int32_t;
using String = std::string_view;

struct ObjectType;
struct ListType;
using GraphQLInputTypes = std::variant<
  Int,
  std::string,
  cgqlSPtr<ObjectType>,
  cgqlSPtr<ListType>
>;

struct ObjectType {
  std::unordered_map<std::string, GraphQLInputTypes> fields;
};

struct ListType {
  cgqlContainer<GraphQLInputTypes> elements;
};

using GraphQLReturnTypes = std::variant<
  Int,
  String
>;

struct ResultMap;
struct Location {
  uint16_t line;
  uint16_t column;
};

using Data = std::variant<
  GraphQLReturnTypes,
  cgqlSPtr<ResultMap>,
  cgqlContainer<GraphQLReturnTypes>,
  cgqlContainer<cgqlSPtr<ResultMap>>,
  std::monostate
>;

class Error {
public:
  Error() = default;
private:
  std::string msg;
  Location location;
};

struct ResultMap {
  std::unordered_map<std::string, Data> data;
  cgqlContainer<Error> errors;
};

using ArgsMap = std::unordered_map<
  std::string,
  GraphQLInputTypes
>;

template<class T, class... Types>
constexpr const T& fromVariant(
  const std::variant<Types...>& v
) {
  return std::get<T>(v);
}

struct Args {
  ArgsMap argsMap;
  void addArg(std::string argName, GraphQLInputTypes arg) {
    argsMap.try_emplace(argName, arg);
  }
  inline const GraphQLInputTypes& operator[](const std::string& argKey) const {
    return this->argsMap.at(argKey);
  }
};

using ResolverFunc = std::function<
  Data(const Args&)
>;

using IsTypeOfFunc = std::function<
  String(const cgqlSPtr<ResultMap>&)
>;

using ResolverMap = std::unordered_map<std::string, ResolverFunc>;
using TypeOfMap = std::unordered_map<std::string, IsTypeOfFunc>;

}
