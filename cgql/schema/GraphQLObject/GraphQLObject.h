#include "GraphQLField.h"
#include <vector>

using std::string;
using std::vector;

class GraphQLObject {
public:
  GraphQLObject(
    string name,
    const vector<GraphQLField>& fields
  );
  ~GraphQLObject();
  inline vector<GraphQLField> getFields() const { return this->fields; };
private:
  string name;
  vector<GraphQLField> fields;
};
