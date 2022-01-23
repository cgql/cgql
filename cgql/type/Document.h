#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "cgql/base/cgqlPch.h"

#include "cgql/logger/logger.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/utils.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/schema/GraphQLTypes.h"

namespace cgql {

namespace internal {

enum OperationType {
  QUERY,
  MUTATION,
  SUBSCRIPTION
};

enum SelectionType {
  BASE,
  FIELD,
  INLINE_FRAGMENT
};

class Argument : public AbstractSchemaTypeDefinition {
public:
  Argument() = default;
  void setValue(const Arg& value) {
    this->value = value;
  }
  const Arg& getValue() const {
    return this->value;
  }
private:
  Arg value;
  std::optional<Location> location;
};

class Selection;
using SelectionSet = cgqlContainer<cgqlSPtr<Selection>>;

class Selection {
public:
  Selection() = default;
  ~Selection() {}
  void setSelectionSet(SelectionSet& selectionSet) {
    cgqlAssert(
      this->selectionSet.size() != 0,
      "selectionSet already contains fields"
    );
    this->selectionSet.swap(selectionSet);
  }
  const SelectionSet& getSelectionSet() const {
    return this->selectionSet;
  }
  void setSelectionType(const SelectionType& type) {
    this->type = type;
  }
  const SelectionType& getSelectionType() const {
    return this->type;
  }
private:
  SelectionType type = SelectionType::BASE;
  SelectionSet selectionSet;
};

class Field
  : public Selection,
    public AbstractSchemaTypeDefinition {
public:
  Field() {
    this->setSelectionType(SelectionType::FIELD);
  };
  ~Field() {}
  void setAlias(const std::string& alias) {
    cgqlAssert(
      this->getName() == alias,
      "field should contain an alias different from its name"
    );
    this->alias = alias;
  }
  const std::string& getAlias() const {
    return this->alias;
  }
  void addArgs(const Argument& arg) {
    this->args.emplace_back(arg);
  }
  const cgqlContainer<Argument>& getArgs() const {
    return this->args;
  }
private:
  std::string alias;
  cgqlContainer<Argument> args;
  std::optional<Location> location;
};

class InlineFragment : public Selection {
public:
  InlineFragment() {
    this->setSelectionType(SelectionType::INLINE_FRAGMENT);
  };
  ~InlineFragment() {}
  void setTypeCondition(const std::string& typeCondition) {
    this->typeCondition = typeCondition;
  }
  const std::string& getTypeCondition() const {
    return this->typeCondition;
  }
private:
  std::string typeCondition;
};

class OperationDefinition {
public:
  OperationDefinition(
    const OperationType& operationType,
    const SelectionSet& selectionSet
  );
  OperationDefinition() = default;
  ~OperationDefinition();
  const OperationType& getOperationType() const { return this->operationType; }
  const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

using Definition = std::variant<
  OperationDefinition,
  cgqlSPtr<TypeDefinition>
>;

class Document {
public:
  Document(
    const cgqlContainer<Definition>& definitions
  );
  Document() = default;
  ~Document();
  cgqlContainer<Definition>& getDefinitions() const { return this->definitions; }
private:
  mutable cgqlContainer<Definition> definitions;
};
} // internal

void printDocumentNode(const internal::Document& doc);
void printResultMap(const ResultMap& rm, uint8_t level = 0);
} // cgql

#endif
