#include "../../cgqlPch.h"

#include "GraphQLField.h"

using std::string;
using std::vector;

class GraphQLObject {
public:
  GraphQLObject(
    string name,
    const vector<GraphQLField>& fields
  );
  ~GraphQLObject();
  inline const string& getName() const { return this->name; };
  inline const vector<GraphQLField>& getFields() const { return this->fields; };
private:
  string name;
  vector<GraphQLField> fields;
};
