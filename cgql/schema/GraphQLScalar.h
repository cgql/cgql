#ifndef GRAPHQL_SCALAR_H
#define GRAPHQL_SCALAR_H

#include "cgql/cgqlPch.h"
#include "cgql/logger/logger.h"

namespace cgql {

template<typename T>
using SerializeFunc = std::function<T(T)>;

typedef int32_t Int;
typedef std::string_view String;

template<typename T>
class GraphQLTypesBase {
public:
  GraphQLTypesBase(
    const std::string& name,
    const SerializeFunc<T>& serialize
  ): name(name), serialize(serialize) {};
  GraphQLTypesBase() = default;
  ~GraphQLTypesBase() {};
  const std::string& getTypeName() const { return this->name; };
  const SerializeFunc<T>& getSerializer() const {
    return this->serialize;
  };
private:
  SerializeFunc<T> serialize;
  std::string name;
};

} // cgql

#endif
