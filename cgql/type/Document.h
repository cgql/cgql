#include <string>
#include <variant>
#include <vector>

using std::vector;
using std::variant;
using std::string;

enum OperationType {
  QUERY,
  MUTATION,
  SUBSCRIPTION
};

class Field;

using Selection = variant<
  Field*
>;
typedef vector<Selection> SelectionSet;

class Field {
public:
  Field(
    const string& name,
    const SelectionSet& selectionSet
  );
  ~Field();
private:
  string name;
  SelectionSet selectionSet;
};

class OperationDefinition {
public:
  OperationDefinition(
    OperationType operationType,
    const SelectionSet& selectionSet
  );
  ~OperationDefinition();
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

class Document {
public:
  Document(
    const vector<OperationDefinition>& definitions
  );
  ~Document();
private:
  vector<OperationDefinition> definitions;
};
