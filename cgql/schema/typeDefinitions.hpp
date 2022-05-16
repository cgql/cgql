#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/utilities/assert.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/base/cgqlPch.h"

namespace cgql {

namespace internal {

enum class DefinitionType {
  TYPE_DEFINITION, // default base type

  DEFAULT_WRAP, // wraps field type including types in lists and non-nulls
  LIST,
  NON_NULL,

  // type system locations based on spec
  SCALAR,
  OBJECT,
  FIELD_DEFINITION,
  ARGUMENT_DEFINITION,
  INTERFACE,
  UNION,
  ENUM,
  ENUM_VALUE,
  INPUT_OBJECT,
  INPUT_FIELD_DEFINITION
};

inline std::ostream& operator<<(std::ostream& os, const DefinitionType& type) {
  switch(type) {
    case DefinitionType::TYPE_DEFINITION:
      os << "TYPE_DEFINITION"; break;

    case DefinitionType::DEFAULT_WRAP:
      os << "DEFAULT_WRAP"; break;
    case DefinitionType::LIST:
      os << "LIST"; break;
    case DefinitionType::NON_NULL:
      os << "NON_NULL"; break;

    case DefinitionType::SCALAR:
      os << "SCALAR"; break;
    case DefinitionType::OBJECT:
      os << "OBJECT"; break;
    case DefinitionType::FIELD_DEFINITION:
      os << "FIELD_DEFINITION"; break;
    case DefinitionType::ARGUMENT_DEFINITION:
      os << "ARGUMENT_DEFINITION"; break;
    case DefinitionType::INTERFACE:
      os << "INTERFACE"; break;
    case DefinitionType::UNION:
      os << "UNION"; break;
    case DefinitionType::ENUM:
      os << "ENUM"; break;
    case DefinitionType::ENUM_VALUE:
      os << "ENUM_VALUE"; break;
    case DefinitionType::INPUT_OBJECT:
      os << "INPUT_OBJECT"; break;
    case DefinitionType::INPUT_FIELD_DEFINITION:
      os << "INPUT_FIELD_DEFINITION"; break;
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
  virtual DefinitionType getDefinitionType() const {
    return DefinitionType::TYPE_DEFINITION;
  };
private:
};

class ScalarTypeDefinition : public TypeDefinition {
public:
  ScalarTypeDefinition() = default;
  ScalarTypeDefinition(const std::string& name) {
    this->setName(name);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::SCALAR;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::LIST;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::NON_NULL;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::DEFAULT_WRAP;
  }
private:
  std::weak_ptr<T> innerType;
};

class ArgumentTypeDefinition : public TypeDefinition {
public:
  void setDefaultValue(GraphQLInputTypes value) {
    this->defaultValue = value;
  }
  GraphQLInputTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setArgumentType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getArgumentType() const {
    return this->type;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::ARGUMENT_DEFINITION;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  GraphQLInputTypes defaultValue;
};

class FieldTypeDefinition : public TypeDefinition {
public:
  void setFieldType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getFieldType() const {
    return this->type;
  }
  void addArg(const ArgumentTypeDefinition& arg) {
    this->argDefs.emplace_back(arg);
  }
  cgqlContainer<ArgumentTypeDefinition> getArgs() const {
    return this->argDefs;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::FIELD_DEFINITION;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INTERFACE;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::OBJECT;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::UNION;
  }
  cgqlContainer<cgqlSPtr<TypeDefinition>> getMembers() const {
    return members;
  }
private:
  cgqlContainer<cgqlSPtr<TypeDefinition>> members;
};

struct EnumValueDefinition : public TypeDefinition {
  EnumValueDefinition(std::string description, std::string name) {
    this->setDescription(description);
    this->setName(name);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::ENUM_VALUE;
  }
};

class EnumTypeDefinition : public TypeDefinition {
public:
  void addValue(EnumValueDefinition value) {
    values.emplace_back(value);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::ENUM;
  }
  cgqlContainer<EnumValueDefinition> getValues() const {
    return this->values;
  }
private:
  cgqlContainer<EnumValueDefinition> values;
};

class InputValueDefinition : public TypeDefinition {
public:
  void setDefaultValue(GraphQLInputTypes value) {
    this->defaultValue = value;
  }
  GraphQLInputTypes getDefaultValue() const {
    return this->defaultValue;
  }
  void setInputValueType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition> getInputValueType() const {
    return this->type;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INPUT_FIELD_DEFINITION;
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
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INPUT_OBJECT;
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
      if(!def) continue; // Temporary!!!
      cgqlAssert(
        def->getType() != DefinitionType::TYPE_DEF,
        "Type is empty"
      );
      const cgqlContainer<std::string>& implements = [this](const cgqlSPtr<TypeDefinition>& def) {
        switch(def->getDefinitionType()) {
          case DefinitionType::OBJECT: {
            const cgqlSPtr<ObjectTypeDefinition>& object =
              std::static_pointer_cast<ObjectTypeDefinition>(def);
            if(object->getName() == "Query") {
              this->setQuery(object);
            }
            return object->getImplementedInterfaces();
          }
          case DefinitionType::INTERFACE: {
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
    if(abstractType->getDefinitionType() == DefinitionType::UNION) {
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
