#ifndef GRAPHQL_OBJECT_H
#define GRAPHQL_OBJECT_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLObject/GraphQLField.h"
#include "cgql/type/abstractType.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {

class GraphQLObject : public internal::AbstractTypeDefinition {
public:
  GraphQLObject(
    const char* name,
    const cgqlContainer<GraphQLField>& fields
  );
  GraphQLObject() = default;
  ~GraphQLObject();
  const cgqlContainer<GraphQLField>& getFields() const { return this->fields; };
  cgqlContainer<GraphQLField>& getMutableFields() {
    return this->fields;
  }
private:
  cgqlContainer<GraphQLField> fields;
};

} // cgql

#endif
