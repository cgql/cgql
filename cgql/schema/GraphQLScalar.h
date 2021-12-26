#ifndef GRAPHQL_SCALAR_H
#define GRAPHQL_SCALAR_H

#include "cgql/cgqlPch.h"
#include "cgql/logger/logger.h"

namespace cgql {

template<typename T>
using SerializeFunc = std::function<T(T)>;

typedef int64_t Int;
typedef std::string_view String;

template<typename T>
class GraphQLTypesBase {
public:
  GraphQLTypesBase(
    String name,
    const SerializeFunc<T>& serialize
  ): name(name), serialize(serialize) {};
  GraphQLTypesBase() = default;
  ~GraphQLTypesBase() {};
  SerializeFunc<T> serialize;
  inline String getTypeName() const { return this->name; };
private:
  String name;
};

} // cgql

#endif
