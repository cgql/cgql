#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/base/cgqlPch.h"

namespace cgql {

namespace internal {

enum DefinitionType {
  DEFAULT_WRAP,
  INTERFACE_TYPE,
  INPUT_OBJECT_TYPE,
  OBJECT_TYPE,
  UNION_TYPE,
  ENUM_TYPE,
  TYPE_DEF,

  SCALAR_TYPE,

  LIST_TYPE,
  NON_NULL_TYPE
};

inline std::ostream& operator<<(std::ostream& os, const DefinitionType& type) {
  switch(type) {
    case DEFAULT_WRAP:
      os << "DEFAULT_WRAP"; break;
    case INPUT_OBJECT_TYPE:
      os << "INPUT_OBJECT_TYPE"; break;
    case OBJECT_TYPE:
      os << "OBJECT_TYPE"; break;
    case UNION_TYPE:
      os << "UNION_TYPE"; break;
    case ENUM_TYPE:
      os << "ENUM_TYPE"; break;
    case TYPE_DEF:
      os << "TYPE_DEF"; break;
    case SCALAR_TYPE:
      os << "SCALAR_TYPE"; break;
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
  virtual ~AbstractSchemaTypeDefinition() {}
  void setName(std::string name) {
    this->name = name;
  }
  const std::string& getName() const {
    return this->name;
  }
  void setDescription(std::string description) {
    this->description = description;
  }
  const std::string& getDescription() const {
    return this->description;
  }
private:
  std::string description;
  std::string name;
};

class AbstractTypeDefinition {
public:
  virtual DefinitionType getType() const = 0;
};

class TypeDefinition : public AbstractTypeDefinition, public AbstractSchemaTypeDefinition {
public:
  TypeDefinition() = default;
  TypeDefinition(const std::string& name) {
    this->setName(name);
  }
  DefinitionType getType() const override {
    return DefinitionType::TYPE_DEF;
  }
private:
};

class ScalarTypeDefinition : public TypeDefinition {
public:
  ScalarTypeDefinition() = default;
  ScalarTypeDefinition(const std::string& name) {
    this->setName(name);
  }
  DefinitionType getType() const override {
    return DefinitionType::SCALAR_TYPE;
  }
private:
};

template<typename T>
class ListTypeDefinition : public TypeDefinition {
public:
  ListTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  DefinitionType getType() const override {
    return DefinitionType::LIST_TYPE;
  }
private:
  mutable cgqlSPtr<T> innerType;
};

template<typename T>
class NonNullTypeDefinition : public TypeDefinition {
public:
  NonNullTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  DefinitionType getType() const override {
    return DefinitionType::NON_NULL_TYPE;
  }
private:
  mutable cgqlSPtr<T> innerType;
};

template<typename T>
class DefaultWrapTypeDefinition : public TypeDefinition {
public:
  DefaultWrapTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
  }
  cgqlSPtr<T> getInnerType() const {
    return cgqlSMakePtr<T>(*innerType);
  };
  DefinitionType getType() const override {
    return DefinitionType::DEFAULT_WRAP;
  }
private:
  mutable cgqlWeakPtr<T> innerType;
};

class ArgumentTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  void setDefaultValue(GraphQLReturnTypes value) {
    this->defaultValue = value;
  }
  GraphQLReturnTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition>& getType() const {
    return this->type;
  }
private:
  mutable cgqlSPtr<TypeDefinition> type;
  GraphQLReturnTypes defaultValue;
};

class FieldTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  void setType(cgqlSPtr<TypeDefinition> type) {
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
  void addField(const FieldTypeDefinition& field) {
    this->fields.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fields;
  }
  DefinitionType getType() const override {
    return DefinitionType::INTERFACE_TYPE;
  }
  void setImplementedInterfaces(cgqlContainer<std::string> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<std::string>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fields;
  mutable cgqlContainer<std::string> implements;
};

class ObjectTypeDefinition : public TypeDefinition {
public:
  void addField(const FieldTypeDefinition& field) {
    this->fieldDefs.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fieldDefs;
  }
  DefinitionType getType() const override {
    return DefinitionType::OBJECT_TYPE;
  }
  void setImplementedInterfaces(cgqlContainer<std::string> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<std::string>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fieldDefs;
  mutable cgqlContainer<std::string> implements;
};

class UnionTypeDefinition : public TypeDefinition {
public:
  void addMember(const cgqlSPtr<ObjectTypeDefinition>& member) {
    this->members.emplace_back(member);
  }
  DefinitionType getType() const override {
    return DefinitionType::UNION_TYPE;
  }
  cgqlContainer<cgqlSPtr<TypeDefinition>> getMembers() const {
    return members;
  }
private:
  mutable cgqlContainer<cgqlSPtr<TypeDefinition>> members;
};

struct EnumValueDefinition : public AbstractSchemaTypeDefinition {
  EnumValueDefinition(std::string description, std::string name) {
    this->setDescription(description);
    this->setName(name);
  }
};

class EnumTypeDefinition : public TypeDefinition {
public:
  void addValue(EnumValueDefinition value) {
    values.emplace_back(value);
  }
  DefinitionType getType() const override {
    return DefinitionType::ENUM_TYPE;
  }
  cgqlContainer<EnumValueDefinition> getValues() const {
    return this->values;
  }
private:
  mutable cgqlContainer<EnumValueDefinition> values;
};

class InputValueDefinition : public AbstractSchemaTypeDefinition {
public:
  void setDefaultValue(GraphQLReturnTypes value) {
    this->defaultValue = value;
  }
  GraphQLReturnTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition>& getType() const {
    return this->type;
  }
private:
  mutable cgqlSPtr<TypeDefinition> type;
  GraphQLReturnTypes defaultValue;
};

class InputObjectTypeDefinition : public TypeDefinition {
public:
  void addField(InputValueDefinition field) {
    fields.emplace_back(field);
  }
  cgqlContainer<InputValueDefinition> getFields() const {
    return this->fields;
  }
  DefinitionType getType() const override {
    return DefinitionType::INPUT_OBJECT_TYPE;
  } 
private:
  cgqlContainer<InputValueDefinition> fields;
};

using ImplementedInterfaces = std::unordered_map<
  std::string,
  cgqlContainer<cgqlSPtr<TypeDefinition>>
>;

class Schema {
public:
  void setQuery(const cgqlSPtr<ObjectTypeDefinition>& query) {
    this->query = query;
  }
  const cgqlSPtr<ObjectTypeDefinition>& getQuery() const {
    return this->query;
  }
  void setTypeDefMap(
    const std::unordered_map<std::string, cgqlSPtr<TypeDefinition>>& typeDefMap
  ) {
    for(auto const& [key, def] : typeDefMap) {
      cgqlAssert(
        def->getType() == DefinitionType::TYPE_DEF,
        "Type is empty"
      );
      const cgqlContainer<std::string>& implements = [this](const cgqlSPtr<TypeDefinition>& def) {
        switch(def->getType()) {
          case DefinitionType::OBJECT_TYPE: {
            const cgqlSPtr<ObjectTypeDefinition>& object =
              std::static_pointer_cast<ObjectTypeDefinition>(def);
            if(object->getName() == "Query") {
              this->setQuery(object);
            }
            return object->getImplementedInterfaces();
          }
          case DefinitionType::INTERFACE_TYPE: {
            const cgqlSPtr<InterfaceTypeDefinition>& interface =
              std::static_pointer_cast<InterfaceTypeDefinition>(def);
            return interface->getImplementedInterfaces();
          }
          default: return cgqlContainer<std::string>{};
        }
      }(def);
      for(auto const& interface : implements) {
        const auto& it = this->implementedInterfaces.find(interface);
        if(it != this->implementedInterfaces.end()) {
          it->second.emplace_back(def);
        } else {
          cgqlContainer<cgqlSPtr<TypeDefinition>> typeDefVec;
          typeDefVec.reserve(1);
          typeDefVec.emplace_back(def);
          this->implementedInterfaces.try_emplace(
            interface, std::move(typeDefVec)
          );
        }
      }
    }
  }
  template<typename T>
  cgqlContainer<cgqlSPtr<TypeDefinition>> getPossibleTypes(
    const T& abstractType
  ) const {
    if(abstractType->getType() == DefinitionType::UNION_TYPE) {
      return std::dynamic_pointer_cast<UnionTypeDefinition>(abstractType)->getMembers();
    }
    const ImplementedInterfaces::const_iterator& it = this->implementedInterfaces.find(abstractType->getName());
    return it->second;
  }
private:
  cgqlSPtr<ObjectTypeDefinition> query;
  ImplementedInterfaces implementedInterfaces;
};

} // end of internal
} // end of cgql
