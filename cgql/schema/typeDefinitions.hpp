#ifndef TYPE_DEFINITIONS_HPP
#define TYPE_DEFINITIONS_HPP

#include "cgql/utilities/cgqlDefs.h"
#include "cgql/base/cgqlPch.h"

namespace cgql {

typedef int32_t Int;
typedef std::string_view String;

namespace internal {

enum DefinitionType : uint8_t {
  INTERFACE_TYPE,
  OBJECT_TYPE,
  TYPE_DEF,

  INT_TYPE,
  STRING_TYPE,

  LIST_TYPE,
  NON_NULL_TYPE
};

inline std::ostream& operator<<(std::ostream& os, const DefinitionType& type) {
  switch(type) {
    case OBJECT_TYPE:
      os << "OBJECT_TYPE"; break;
    case TYPE_DEF:
      os << "TYPE_DEF"; break;
    case INT_TYPE:
      os << "INT_TYPE"; break;
    case STRING_TYPE:
      os << "STRING_TYPE"; break;
    case LIST_TYPE:
      os << "LIST_TYPE"; break;
    case NON_NULL_TYPE:
      os << "NON_NULL_TYPE"; break;
    case INTERFACE_TYPE:
      os << "INTERFACE_TYPE"; break;
  }
  return os;
}

class AbstractSchemaTypeDefinition {
public:
  void setName(const std::string& name) {
    this->name = name;
  }
  const std::string& getName() const {
    return this->name;
  }
private:
  std::string name;
};

class AbstractTypeDefinition {
public:
  virtual const DefinitionType& getType() const = 0;
  virtual void setEnumType(const DefinitionType& type) = 0;
};

class TypeDefinition : public AbstractTypeDefinition, public AbstractSchemaTypeDefinition {
public:
  TypeDefinition() {
    this->type = DefinitionType::TYPE_DEF;
  };
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  DefinitionType type;
};

template<typename T>
class ScalarTypeDefinition : public TypeDefinition {
public:
  ScalarTypeDefinition(
    const std::string& name,
    const DefinitionType& type
  ) {
    this->setName(name);
    this->type = type;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  DefinitionType type;
};

template<typename T>
class ListTypeDefinition : public TypeDefinition {
public:
  ListTypeDefinition(const cgqlSPtr<T>& innerType)
    : innerType(innerType) {
    this->type = DefinitionType::LIST_TYPE;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  mutable cgqlSPtr<T> innerType;
  DefinitionType type;
};

template<typename T>
class NonNullTypeDefinition : public TypeDefinition {
public:
  NonNullTypeDefinition(const cgqlSPtr<T>& innerType)
    : innerType(innerType) {
    this->type = DefinitionType::NON_NULL_TYPE;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  mutable cgqlSPtr<T> innerType;
  DefinitionType type;
};

namespace BuiltinTypes {
  inline cgqlSPtr<ScalarTypeDefinition<Int>> IntType =
    cgqlSMakePtr<ScalarTypeDefinition<Int>>("Int", DefinitionType::INT_TYPE);
  inline cgqlSPtr<ScalarTypeDefinition<String>> StringType =
    cgqlSMakePtr<ScalarTypeDefinition<String>>("String", DefinitionType::STRING_TYPE);
} // end of BuiltinTypes

class ArgumentTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  ArgumentTypeDefinition() = default;
  ~ArgumentTypeDefinition() {}
  void setType(const cgqlSPtr<TypeDefinition>& type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition>& getType() const {
    return this->type;
  }
private:
  mutable cgqlSPtr<TypeDefinition> type;
};

class FieldTypeDefinition;
class ObjectTypeDefinition : public TypeDefinition {
public:
  ObjectTypeDefinition() {
    this->type = DefinitionType::OBJECT_TYPE;
  };
  ~ObjectTypeDefinition() {}
  void addField(const FieldTypeDefinition& field) {
    this->fieldDefs.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fieldDefs;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fieldDefs;
  DefinitionType type;
};

class FieldTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  FieldTypeDefinition() = default;
  ~FieldTypeDefinition() {}
  void setType(const cgqlSPtr<TypeDefinition>& type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition>& getType() const {
    return this->type;
  }
  void addArg(const ArgumentTypeDefinition& arg) {
    this->argDefs.emplace_back(arg);
  }
  cgqlContainer<ArgumentTypeDefinition>& getArgs() const {
    return this->argDefs;
  }
private:
  mutable cgqlSPtr<TypeDefinition> type;
  mutable cgqlContainer<ArgumentTypeDefinition> argDefs;
};

class InterfaceTypeDefinition : public TypeDefinition {
public:
  InterfaceTypeDefinition() {
    this->type = DefinitionType::INTERFACE_TYPE;
  };
  ~InterfaceTypeDefinition() {}
  void addField(const FieldTypeDefinition& field) {
    this->fields.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fields;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setEnumType(const DefinitionType& type) override {
    this->type = type;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fields;
  mutable cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> implements;
  DefinitionType type;
};


class Schema {
public:
  Schema() = default;
  ~Schema() {}
  void setQuery(const cgqlSPtr<ObjectTypeDefinition>& query) {
    this->query = query;
  }
  const cgqlSPtr<ObjectTypeDefinition>& getQuery() const {
    return this->query;
  }
private:
  cgqlSPtr<ObjectTypeDefinition> query;
};

} // end of internal
} // end of cgql

#endif /* end of include guard: TYPE_DEFINITIONS_HPP */
