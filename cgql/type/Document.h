#pragma once

#include "cgql/base/cgqlPch.h"

#include "cgql/logger/logger.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/utils.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/schema/GraphQLTypes.h"

namespace cgql {

namespace internal {

enum class OperationType {
  QUERY,
  MUTATION,
  SUBSCRIPTION
};

enum class SelectionType {
  BASE,
  FIELD,
  INLINE_FRAGMENT,
  FRAGMENT
};

class Argument : public AbstractSchemaTypeDefinition {
public:
  void setValue(GraphQLInputTypes value) {
    this->value = value;
  }
  const GraphQLInputTypes& getValue() const {
    return this->value;
  }
private:
  GraphQLInputTypes value;
};

class Selection;
using SelectionSet = cgqlContainer<cgqlSPtr<Selection>>;

using GroupedField = std::map<
  std::string,
  SelectionSet
>;
class Selection {
public:
  virtual ~Selection() {}
  void setSelectionSet(SelectionSet selectionSet) {
    this->selectionSet = std::move(selectionSet);
  }
  const SelectionSet& getSelectionSet() const {
    return this->selectionSet;
  }
  void setSelectionType(SelectionType type) {
    this->type = type;
  }
  SelectionType getSelectionType() const {
    return this->type;
  }
private:
  SelectionSet selectionSet;
  SelectionType type = SelectionType::BASE;
};

class Field
  : public Selection,
    public AbstractSchemaTypeDefinition {
public:
  Field() {
    this->setSelectionType(SelectionType::FIELD);
  };
  void setAlias(std::string alias) {
    this->alias = alias;
  }
  const std::string& getAlias() const {
    return this->alias;
  }
  void addArgs(Argument arg) {
    this->args.emplace_back(arg);
  }
  const cgqlContainer<Argument>& getArgs() const {
    return this->args;
  }
  const std::string& getResponseKey() const {
    return this->alias.empty() ? this->getName() : this->alias;
  }
private:
  std::string alias;
  cgqlContainer<Argument> args;
};

class InlineFragment : public Selection {
public:
  InlineFragment() {
    this->setSelectionType(SelectionType::INLINE_FRAGMENT);
  };
  void setTypeCondition(std::string typeCondition) {
    this->typeCondition = typeCondition;
  }
  const std::string& getTypeCondition() const {
    return this->typeCondition;
  }
private:
  std::string typeCondition;
};

class Fragment
  : public Selection,
    public AbstractSchemaTypeDefinition {
public:
  Fragment() {
    this->setSelectionType(SelectionType::FRAGMENT);
  }
};

class OperationDefinition {
public:
  OperationDefinition(
    OperationType operationType,
    SelectionSet selectionSet
  );
  OperationDefinition() = default;
  ~OperationDefinition();
  OperationType getOperationType() const { return this->operationType; }
  const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

class FragmentDefinition : public AbstractSchemaTypeDefinition  {
public:
  void setTypeCondition(std::string typeCondition) {
    this->typeCondition = typeCondition;
  }
  const std::string& getTypeCondition() const {
    return this->typeCondition;
  }
  void setSelectionSet(SelectionSet selectionSet) {
    this->selectionSet = std::move(selectionSet);
  }
  const SelectionSet& getSelectionSet() const {
    return this->selectionSet;
  }
private:
  std::string typeCondition;
  SelectionSet selectionSet;
};

using Definition = std::variant<
  OperationDefinition,
  FragmentDefinition
>;

class Document {
public:
  Document(
    cgqlContainer<Definition> definitions
  );
  Document() = default;
  ~Document();
  const cgqlContainer<Definition>& getDefinitions() const { return this->definitions; }
private:
  cgqlContainer<Definition> definitions;
};
} // internal

void printDocumentNode(const internal::Document& doc);
void printResultMap(const Object& obj, uint8_t level = 0);
} // cgql
