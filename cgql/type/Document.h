#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../cgqlPch.h"

#include "../logger/logger.h"
#include "../schema/GraphQLDefinition.h"

namespace cgql {
namespace internal {

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
  inline const string getName() const { return this->name; }
  inline const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  string name;
  SelectionSet selectionSet;
};

class OperationDefinition {
public:
  OperationDefinition(
    const OperationType& operationType,
    const SelectionSet& selectionSet
  );
  ~OperationDefinition();
  inline const OperationType& getOperationType() const { return this->operationType; }
  inline const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

using TypeDefinition = variant<
  GraphQLObject
>;

using Definition = variant<
  OperationDefinition,
  TypeDefinition
>;

class Document {
public:
  Document(
    const vector<Definition>& definitions
  );
  ~Document();
  inline const vector<Definition>& getDefinitions() const { return this->definitions; }
private:
  vector<Definition> definitions;
};
} // internal

void printSelectionSet(internal::SelectionSet selectionSet, int level);
void printDocumentNode(internal::Document& doc);
void printResultMap(const ResultMap& rm, uint8_t level = 0);
} // cgql

#endif
