#include <functional>
#include <string>

using std::string;
using std::function;

template<typename T>
using SerializeFunc = function<T(T)>;

typedef int64_t Int;
typedef std::string String;

template<typename T>
class GraphQLScalarType {
public:
  GraphQLScalarType(
    const string& name,
    const SerializeFunc<T>& serialize
  ): name(name), serialize(serialize) {};
  ~GraphQLScalarType() {};
private:
  string name;
  SerializeFunc<T> serialize;
};
