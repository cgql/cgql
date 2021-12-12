#ifndef GRAPHQL_SCALAR_H
#define GRAPHQL_SCALAR_H

#include "../cgqlPch.h"

using std::string;
using std::function;

template<typename T>
using SerializeFunc = function<T(T)>;

typedef int64_t Int;
typedef std::string_view String;

template<typename T>
class GraphQLTypesBase {
public:
  GraphQLTypesBase(
    const string& name,
    const SerializeFunc<T>& serialize
  ): name(name), serialize(serialize) {};
  ~GraphQLTypesBase() {};
  SerializeFunc<T> serialize;
  inline string getTypeName() const { return this->name; };
private:
  string name;
};

#endif
