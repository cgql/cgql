#ifndef GRAPHQL_TYPES_H
#define GRAPHQL_TYPES_H

#include "cgql/cgqlPch.h"
#include "cgql/schema/GraphQLScalar.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/utilities/assert.h"

namespace cgql {

class GraphQLObject;

namespace GraphQLTypes {
  inline GraphQLTypesBase<Int> GraphQLInt(
    "Int",
    [](const Int& value) -> const Int& {
      bool isNotInLimits =
        value > internal::GRAPHQL_INT_LIMITS::MAX ||
        value < internal::GRAPHQL_INT_LIMITS::MIN;
      if(isNotInLimits) {
        cgqlAssert(
          !isNotInLimits,
          "Value should be less than 2147483647 and greater than -2147483648"
        );
      }
      return value;
    }
  );
  inline GraphQLTypesBase<String> GraphQLString(
    "String",
    [](const String& value) -> const String& {
      return value;
    }
  );
}

using GraphQLScalarTypes = std::variant<
  GraphQLTypesBase<Int>,
  GraphQLTypesBase<String>,
  cgqlSPtr<GraphQLObject>
>;

using GraphQLReturnTypes = std::variant<
  Int,
  String,
  cgqlSPtr<GraphQLObject>
>;

}

namespace cgql {

namespace internal {
  class Field;
  typedef std::unordered_map<
    std::string,
    cgqlContainer<Field>
  > GroupedField;
}

struct ResultMap;
struct Location {
  uint16_t line;
  uint16_t column;
};

typedef std::variant<
  GraphQLReturnTypes,
  cgqlSPtr<ResultMap>,
  cgqlContainer<GraphQLReturnTypes>,
  cgqlContainer<cgqlSPtr<ResultMap>>,
  std::monostate
> Data;

class Error {
public:
  Error() = default;
private:
  std::string msg;
  Location location;
};

struct ResultMap {
  std::unordered_map<std::string, Data> data;
  std::list<Error> errors;
};

typedef std::variant<
  Int,
  String
> Arg;

typedef std::unordered_map<
  std::string,
  Arg
> ArgsMap;

struct Args {
  ArgsMap argsMap;
  inline const Arg& operator[](const std::string& argKey) const {
    return this->argsMap.at(argKey);
  }
};

template<class T, class... Types>
constexpr const T& fromVariant(
  const std::variant<Types...>& v
) {
  return std::get<T>(v);
}

using ResolverFunc = std::function<
  Data(const Args&)
>;

typedef std::unordered_map<std::string, ResolverFunc> ResolverMap;

}


#endif
