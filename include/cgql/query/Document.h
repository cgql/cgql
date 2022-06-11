#pragma once

#include "cgql/schema/typeDefinitions.h"
#include "cgql/schema/GraphQLTypes.h"

namespace cgql {

enum class OperationType {
  QUERY,
  MUTATION,
  SUBSCRIPTION
};

enum class SelectionType {
  FIELD,
  INLINE_FRAGMENT,
  FRAGMENT
};

class Argument : public AbstractSchemaTypeDefinition {
public:
  void setValue(GraphQLInputTypes value);
  const GraphQLInputTypes& getValue() const;
private:
  GraphQLInputTypes value;
};

class Selection;
using SelectionSet = cgqlContainer<cgqlSPtr<Selection>>;

class Selection {
public:
  virtual ~Selection();
  void setSelectionSet(SelectionSet selectionSet);
  const SelectionSet& getSelectionSet() const;
  void setSelectionType(SelectionType type);
  SelectionType getSelectionType() const;
private:
  SelectionSet selectionSet;
  SelectionType type;
};

class Field
  : public Selection,
    public AbstractSchemaTypeDefinition {
public:
  Field();
  void setAlias(std::string alias);
  const std::string& getAlias() const;
  void addArgs(Argument arg);
  const cgqlContainer<Argument>& getArgs() const;
  const std::string& getResponseKey() const;
private:
  std::string alias;
  cgqlContainer<Argument> args;
};

class InlineFragment : public Selection {
public:
  InlineFragment();
  void setTypeCondition(std::string typeCondition);
  const std::string& getTypeCondition() const;
private:
  std::string typeCondition;
};

class Fragment
  : public Selection,
    public AbstractSchemaTypeDefinition {
public:
  Fragment();
};

class OperationDefinition {
public:
  OperationDefinition(
    SelectionSet selectionSet,
    OperationType operationType
  );
  OperationDefinition();
  const SelectionSet& getSelectionSet() const;
  OperationType getOperationType() const;
private:
  SelectionSet selectionSet;
  OperationType operationType;
};

class FragmentDefinition : public AbstractSchemaTypeDefinition  {
public:
  void setTypeCondition(std::string typeCondition);
  const std::string& getTypeCondition() const;
  void setSelectionSet(SelectionSet selectionSet);
  const SelectionSet& getSelectionSet() const;
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
  Document(cgqlContainer<Definition> definitions);
  const cgqlContainer<Definition>& getDefinitions() const;
private:
  cgqlContainer<Definition> definitions;
};

void printDocumentNode(const Document& doc);
void printResultMap(const Object& obj, uint8_t level = 0);
} // cgql
