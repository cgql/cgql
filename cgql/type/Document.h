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
  inline string getName() { return this->name; }
  inline SelectionSet getSelectionSet() { return this->selectionSet; }
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
  inline OperationType getOperationType() { return this->operationType; }
  inline SelectionSet getSelectionSet() { return this->selectionSet; }
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
  inline vector<OperationDefinition> getDefinitions() { return this->definitions; }
private:
  vector<OperationDefinition> definitions;
};
