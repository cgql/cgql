#include "../../utils/pch.h"
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
private:
  string name;
  vector<GraphQLField> fields;
};
