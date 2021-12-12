#ifndef GRAPHQL_OBJECT_H
#define GRAPHQL_OBJECT_H

#include "../../cgqlPch.h"

#include "GraphQLField.h"

using std::string;
using std::vector;

class GraphQLObject {
public:
  GraphQLObject(
    const char* name,
    const vector<GraphQLField>& fields
  );
  ~GraphQLObject();
  inline const string& getName() const { return this->name; };
  inline const vector<GraphQLField>& getFields() const { return this->fields; };
  inline const GraphQLField findFieldByName(string name) const {
    for(auto field : this->getFields()) {
      if(field.getName() == name) {
        return field;
      }
    }
    throw name;
  };
private:
  string name;
  vector<GraphQLField> fields;
};

#endif
