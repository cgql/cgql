#ifndef GRAPHQL_DEFINITION_H
#define GRAPHQL_DEFINITION_H
#include "cgql/schema/GraphQLObject/GraphQLObject.h"

namespace cgql {

class GraphQLSchema {
public:
  GraphQLSchema(
    GraphQLObject& query
  ): query(query) {};
  GraphQLSchema() = default;
  ~GraphQLSchema() {};
  inline const GraphQLObject& getQuery() const {
    return this->query;
  }
  inline void setQuery(const GraphQLObject& query) {
    this->query = query;
  }
private:
  GraphQLObject query;
};

} // cgql

#endif
