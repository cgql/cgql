#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "cgql/cgqlPch.h"

#include "cgql/logger/logger.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/utils.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/schema/GraphQLTypes.h"

namespace cgql {

struct TypeMetaData {
public:
  void setWrappedInnerType(const cgqlSPtr<TypeMetaData>& wrappedInnerType);
  void setIsList(bool isList);
  void setIsNonNull(bool isNonNull);
  const cgqlSPtr<TypeMetaData>& getWrappedInnerType() const;
  bool isList() const;
  bool isNonNull() const;
private:
  cgqlSPtr<TypeMetaData> wrappedInnerType;
  bool _isList;
  bool _isNonNull;
};

namespace internal {

enum OperationType {
  QUERY,
  MUTATION,
  SUBSCRIPTION
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

class Field;

using Selection = std::variant<
  cgqlSPtr<Field>
>;
using SelectionSet = cgqlContainer<Selection>;

class Field : public AbstractSchemaTypeDefinition {
public:
  Field(
    const std::string& name,
    const SelectionSet& selectionSet
  );
  Field() = default;
  ~Field();
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
  void setSelectionSet(const SelectionSet& selectionSet) {
    cgqlAssert(
      this->selectionSet.size() != 0,
      "selectionSet already contains fields"
    );
    this->selectionSet = std::move(selectionSet);
  }
  const SelectionSet& getSelectionSet() const { return this->selectionSet; }
  void addArgs(const Argument& arg) {
    this->args.push_back(arg);
  }
  const cgqlContainer<Argument>& getArgs() const {
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
  const OperationType& getOperationType() const { return this->operationType; }
  const SelectionSet& getSelectionSet() const { return this->selectionSet; }
private:
  OperationType operationType;
  SelectionSet selectionSet;
};

class Type : public AbstractSchemaTypeDefinition {
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
  bool operator==(const Type& other) const {
    return (
      this->_isList == other._isList &&
      this->_isNonNull == other._isNonNull &&
      this->wrappedInnerType == other.wrappedInnerType &&
      this->getName() == other.getName()
    );
  }
  bool isList() const {
    return this->_isList;
  }
  bool isNonNull() const {
    return this->_isNonNull;
  }
  const std::optional<cgqlSPtr<Type>>& getWrappedInnerType() const {
    return this->wrappedInnerType;
  }
  void setTypeList(bool isList) {
    this->_isList = isList;
  }
  void setTypeNonNull(bool isNonNull) {
    this->_isNonNull = isNonNull;
  }
  void setWrappedInnerType(const Type& innerType) {
    this->wrappedInnerType = cgqlSMakePtr<Type>(innerType);
  }
private:
  std::optional<cgqlSPtr<Type>> wrappedInnerType;
  bool _isList = false;
  bool _isNonNull = false;
  std::optional<Location> location;
};

using RootTypeDefinition = std::variant<
  ObjectTypeDefinition
>;

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
