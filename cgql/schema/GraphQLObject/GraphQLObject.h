#ifndef GRAPHQL_OBJECT_H
#define GRAPHQL_OBJECT_H

#include "../../cgqlPch.h"

#include "GraphQLField.h"
#include "../../type/abstractType.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

using std::string;

class GraphQLObject : public internal::AbstractTypeDefinition {
public:
  GraphQLObject(
    const char* name,
    const cgqlContainer<GraphQLField>& fields
  );
  GraphQLObject() = default;
  ~GraphQLObject();
  inline const cgqlContainer<GraphQLField>& getFields() const { return this->fields; };
  inline cgqlContainer<GraphQLField>& getMutableFields() {
    return this->fields;
  }
private:
  cgqlContainer<GraphQLField> fields;
};

} // cgql

#endif
