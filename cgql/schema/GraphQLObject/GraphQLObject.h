#ifndef GRAPHQL_OBJECT_H
#define GRAPHQL_OBJECT_H

#include "../../cgqlPch.h"

#include "GraphQLField.h"

namespace cgql {

using std::string;
using std::vector;

class GraphQLObject {
public:
  GraphQLObject(
    const char* name,
    const vector<GraphQLField>& fields
  );
  GraphQLObject() = default;
  ~GraphQLObject();
  inline const string& getName() const { return this->name; };
  inline const vector<GraphQLField>& getFields() const { return this->fields; };
  inline vector<GraphQLField>& getMutableFields() {
    return this->fields;
  }
private:
  string name;
  vector<GraphQLField> fields;
};

} // cgql

#endif
