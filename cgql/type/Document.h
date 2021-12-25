#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../cgqlPch.h"

#include "../logger/logger.h"
#include "../schema/GraphQLDefinition.h"
#include "cgql/utilities/assert.h"

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

class Argument : public AbstractTypeDefinition {
public:
  Argument() = default;
  inline void setValue(const Arg& value) {
    this->value = value;
  }
  inline const Arg& getValue() const {
    return this->value;
  }
private:
  Arg value;
};

class Field;

using Selection = variant<
  cgqlSPtr<Field>
>;
typedef vector<Selection> SelectionSet;

class Field : public AbstractTypeDefinition {
public:
  Field(
    const string& name,
    const SelectionSet& selectionSet
  );
  Field() = default;
  ~Field();
  inline void setAlias(const string& alias) {
    cgqlAssert(
      this->name == alias,
      "field should contain an alias different from its name"
    );
    this->alias = alias;
  }
  inline const string& getAlias() const {
    return this->alias;
  }
  inline void setSelectionSet(const SelectionSet& selectionSet) {
    cgqlAssert(
      this->selectionSet.size() != 0,
      "selectionSet already contains fields"
    );
    this->selectionSet = std::move(selectionSet);
  }
  inline const SelectionSet& getSelectionSet() const { return this->selectionSet; }
  inline void addArgs(const Argument& arg) {
    this->args.push_back(arg);
  }
  inline const vector<Argument>& getArgs() const {
    return this->args;
  }
private:
  string alias;
  SelectionSet selectionSet;
  std::vector<Argument> args;
};

class OperationDefinition {
public:
  OperationDefinition(
    const OperationType& operationType,
    const SelectionSet& selectionSet
  );
  OperationDefinition() = default;
  ~OperationDefinition();
  inline const OperationType& getOperationType() const { return this->operationType; }
  inline const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

class ArgumentDefinitions : public AbstractTypeDefinition {
public:
  ArgumentDefinitions() = default;
  ~ArgumentDefinitions();
  inline void setType(const string& type) {
    this->type = type;
  }
  inline const string& getType() const {
    return this->type;
  }
private:
  string type;
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
  inline void addArg(const ArgumentDefinitions& arg) {
    this->args.push_back(arg);
  }
  inline const vector<ArgumentDefinitions>& getArgs() const {
    return this->args;
  }
private:
  string type;
  vector<ArgumentDefinitions> args;
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

void printSelectionSet(const internal::SelectionSet selectionSet, int level);
void printDocumentNode(const internal::Document& doc);
void printResultMap(const ResultMap& rm, uint8_t level = 0);
} // cgql

#endif
