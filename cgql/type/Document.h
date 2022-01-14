#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "cgql/cgqlPch.h"

#include "cgql/logger/logger.h"
#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/utils.h"

namespace cgql {
namespace internal {

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
  std::optional<Location> location;
};

class Field;

using Selection = std::variant<
  cgqlSPtr<Field>
>;
using SelectionSet = cgqlContainer<Selection>;

class Field : public AbstractTypeDefinition {
public:
  Field(
    const std::string& name,
    const SelectionSet& selectionSet
  );
  Field() = default;
  ~Field();
  inline void setAlias(const std::string& alias) {
    cgqlAssert(
      this->name == alias,
      "field should contain an alias different from its name"
    );
    this->alias = alias;
  }
  inline const std::string& getAlias() const {
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
  inline const cgqlContainer<Argument>& getArgs() const {
    return this->args;
  }
private:
  std::string alias;
  SelectionSet selectionSet;
  cgqlContainer<Argument> args;
  std::optional<Location> location;
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

class Type : public AbstractTypeDefinition {
public:
  Type(
    const std::string& name,
    bool isList = false,
    bool isNonNull = false
  ): _isList(isList), _isNonNull(isNonNull) {
    this->setName(name);
  }
  Type() = default;
  ~Type() {}
  inline bool isList() const {
    return this->_isList;
  }
  inline bool isNonNull() const {
    return this->_isNonNull;
  }
  inline const std::optional<cgqlSPtr<Type>>& getWrappedInnerType() const {
    return this->wrappedInnerType;
  }
  inline void setTypeList(bool isList) {
    this->_isList = isList;
  }
  inline void setTypeNonNull(bool isNonNull) {
    this->_isNonNull = isNonNull;
  }
  inline void setWrappedInnerType(const Type& innerType) {
    this->wrappedInnerType = cgqlSMakePtr<Type>(innerType);
  }
  bool operator==(const Type& other) const {
    return (
      this->_isList == other._isList &&
      this->_isNonNull == other._isNonNull &&
      this->name == this->name
    );
  }
private:
  std::optional<cgqlSPtr<Type>> wrappedInnerType;
  bool _isList = false;
  bool _isNonNull = false;
  std::optional<Location> location;
};

class ArgumentDefinitions : public AbstractTypeDefinition {
public:
  ArgumentDefinitions() = default;
  ~ArgumentDefinitions();
  inline void setType(const Type& type) {
    this->type = type;
  }
  inline const Type& getType() const {
    return this->type;
  }
private:
  Type type;
  std::optional<Location> location;
};

class FieldDefinition : public AbstractTypeDefinition {
public:
  FieldDefinition() = default;
  ~FieldDefinition();
  inline void setType(const Type& type) {
    this->type = type;
  }
  inline const Type& getType() const {
    return this->type;
  }
  inline void addArg(const ArgumentDefinitions& arg) {
    this->args.push_back(arg);
  }
  inline const cgqlContainer<ArgumentDefinitions>& getArgs() const {
    return this->args;
  }
private:
  Type type;
  cgqlContainer<ArgumentDefinitions> args;
  std::optional<Location> location;
};

class ObjectTypeDefinition : public AbstractTypeDefinition {
public:
  ObjectTypeDefinition() = default;
  ~ObjectTypeDefinition();
  inline void addField(const FieldDefinition& field) {
    this->fields.push_back(field);
  }
  inline const cgqlContainer<FieldDefinition>& getFields() const {
    return this->fields;
  }
private:
  cgqlContainer<FieldDefinition> fields;
  std::optional<Location> location;
};

using TypeDefinition = std::variant<
  ObjectTypeDefinition
>;

using Definition = std::variant<
  OperationDefinition,
  TypeDefinition
>;

class Document {
public:
  Document(
    const cgqlContainer<Definition>& definitions
  );
  Document() = default;
  ~Document();
  inline const cgqlContainer<Definition>& getDefinitions() const { return this->definitions; }
private:
  cgqlContainer<Definition> definitions;
};
} // internal

void printDocumentNode(const internal::Document& doc);
void printResultMap(const ResultMap& rm, uint8_t level = 0);
} // cgql

namespace std {
  template<>
  struct hash<cgql::internal::Type> {
    std::size_t operator()(const cgql::internal::Type& type) const {
      std::size_t hashIsList = std::hash<bool>()(type.isList());
      std::size_t hashIsNonNull = std::hash<bool>()(type.isNonNull());
      std::size_t hashName = std::hash<std::string>()(type.getName());
      return hashIsList ^ (hashIsNonNull << 1 ^ (hashName << 2));
    }
  };
};

#endif
