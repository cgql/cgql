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

class FieldDefinition : public AbstractTypeDefinition {
public:
  FieldDefinition() = default;
  ~FieldDefinition();
  inline void setType(const string& type) {
    this->type = type;
  }
  inline const string& getType() const {
    return this->type;
  }
private:
  string type;
};

class ObjectTypeDefinition : public AbstractTypeDefinition {
public:
  ObjectTypeDefinition() = default;
  ~ObjectTypeDefinition();
  inline void addField(const FieldDefinition& field) {
    this->fields.push_back(field);
  }
  inline const vector<FieldDefinition>& getFields() const {
    return this->fields;
  }
private:
  vector<FieldDefinition> fields;
};

using TypeDefinition = variant<
  ObjectTypeDefinition
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
