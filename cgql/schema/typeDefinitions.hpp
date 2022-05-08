#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/base/cgqlPch.h"

namespace cgql {

namespace internal {

enum class DefinitionType {
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
    case DefinitionType::DEFAULT_WRAP:
      os << "DEFAULT_WRAP"; break;
    case DefinitionType::INPUT_OBJECT_TYPE:
      os << "INPUT_OBJECT_TYPE"; break;
    case DefinitionType::OBJECT_TYPE:
      os << "OBJECT_TYPE"; break;
    case DefinitionType::UNION_TYPE:
      os << "UNION_TYPE"; break;
    case DefinitionType::ENUM_TYPE:
      os << "ENUM_TYPE"; break;
    case DefinitionType::TYPE_DEF:
      os << "TYPE_DEF"; break;
    case DefinitionType::SCALAR_TYPE:
      os << "SCALAR_TYPE"; break;
    case DefinitionType::LIST_TYPE:
      os << "LIST_TYPE"; break;
    case DefinitionType::NON_NULL_TYPE:
      os << "NON_NULL_TYPE"; break;
    case DefinitionType::INTERFACE_TYPE:
      os << "INTERFACE_TYPE"; break;
  }
  return os;
}

class AbstractSchemaTypeDefinition {
public:
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
protected:
  std::string name;
  std::string description;
};

class TypeDefinition : public AbstractSchemaTypeDefinition {
public:
  TypeDefinition() = default;
  TypeDefinition(const std::string& name) {
    this->setName(name);
  }
  virtual ~TypeDefinition() = default;
  virtual DefinitionType getType() const {
    return DefinitionType::TYPE_DEF;
  };
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
  cgqlSPtr<T> getInnerType() const {
    return this->innerType;
  };
  DefinitionType getType() const override {
    return DefinitionType::LIST_TYPE;
  }
private:
  cgqlSPtr<T> innerType;
};

template<typename T>
class NonNullTypeDefinition : public TypeDefinition {
public:
  NonNullTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
  }
  cgqlSPtr<T> getInnerType() const {
    return this->innerType;
  };
  DefinitionType getType() const override {
    return DefinitionType::NON_NULL_TYPE;
  }
private:
  cgqlSPtr<T> innerType;
};

template<typename T>
class DefaultWrapTypeDefinition : public TypeDefinition {
public:
  DefaultWrapTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
  }
  cgqlSPtr<T> getInnerType() const {
    return innerType.lock();
  };
  DefinitionType getType() const override {
    return DefinitionType::DEFAULT_WRAP;
  }
private:
  std::weak_ptr<T> innerType;
};

class ArgumentTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  void setDefaultValue(GraphQLInputTypes value) {
    this->defaultValue = value;
  }
  GraphQLInputTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getType() const {
    return this->type;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  GraphQLInputTypes defaultValue;
};

class FieldTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getType() const {
    return this->type;
  }
  DefinitionType getDefType() const {
    return this->type->getType();
  }
  void addArg(const ArgumentTypeDefinition& arg) {
    this->argDefs.emplace_back(arg);
  }
  cgqlContainer<ArgumentTypeDefinition> getArgs() const {
    return this->argDefs;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  cgqlContainer<ArgumentTypeDefinition> argDefs;
};

class InterfaceTypeDefinition : public TypeDefinition {
public:
  void addField(const FieldTypeDefinition& field) {
    this->fields.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition> getFields() const {
    return this->fields;
  }
  DefinitionType getType() const override {
    return DefinitionType::INTERFACE_TYPE;
  }
  void setImplementedInterfaces(cgqlContainer<std::string> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<std::string> getImplementedInterfaces() const {
    return this->implements;
  }
private:
  cgqlContainer<FieldTypeDefinition> fields;
  cgqlContainer<std::string> implements;
};

class ObjectTypeDefinition : public TypeDefinition {
public:
  void addField(const FieldTypeDefinition& field) {
    this->fieldDefs.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition> getFields() const {
    return this->fieldDefs;
  }
  DefinitionType getType() const override {
    return DefinitionType::OBJECT_TYPE;
  }
  void setImplementedInterfaces(cgqlContainer<std::string> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<std::string> getImplementedInterfaces() const {
    return this->implements;
  }
private:
  cgqlContainer<FieldTypeDefinition> fieldDefs;
  cgqlContainer<std::string> implements;
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
  cgqlContainer<cgqlSPtr<TypeDefinition>> members;
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
  cgqlContainer<EnumValueDefinition> values;
};

class InputValueDefinition : public AbstractSchemaTypeDefinition {
public:
  void setDefaultValue(GraphQLInputTypes value) {
    this->defaultValue = value;
  }
  GraphQLInputTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getType() const {
    return this->type;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  GraphQLInputTypes defaultValue;
};

class InputObjectTypeDefinition : public TypeDefinition {
public:
  void addField(const InputValueDefinition& field) {
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
        def->getType() != DefinitionType::TYPE_DEF,
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
