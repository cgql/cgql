#ifndef GRAPHQL_OBJECT_H
#define GRAPHQL_OBJECT_H

#include "../../cgqlPch.h"

#include "GraphQLField.h"
#include "../../type/abstractType.h"

namespace cgql {

using std::string;
using std::vector;

class GraphQLObject : public internal::AbstractTypeDefinition {
public:
  GraphQLObject(
    const char* name,
    const vector<GraphQLField>& fields
  );
  GraphQLObject() = default;
  ~GraphQLObject();
  inline const vector<GraphQLField>& getFields() const { return this->fields; };
  inline vector<GraphQLField>& getMutableFields() {
    return this->fields;
  }
private:
  vector<GraphQLField> fields;
};

} // cgql

#endif
