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
  std::map<std::string, GraphQLInputTypes> fields;
};

struct ListType {
  cgqlContainer<GraphQLInputTypes> elements;
};

struct Object;
struct List;

using Data = std::variant<
  Int,
  String,
  cgqlSPtr<Object>,
  cgqlSPtr<List>,
  std::monostate
>;

struct Object {
  std::map<std::string, Data> fields;
};
struct List {
  std::vector<Data> elements;
};

using ArgsMap = std::map<
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
  const GraphQLInputTypes& operator[](const std::string& argKey) const {
    return this->argsMap.at(argKey);
  }
};

using ResolverFunc = std::function<
  Data(const Args&)
>;

using IsTypeOfFunc = std::function<
  String(const cgqlSPtr<Object>&)
>;

using ResolverMap = std::map<std::string, ResolverFunc>;
using TypeOfMap = std::map<std::string, IsTypeOfFunc>;

}
