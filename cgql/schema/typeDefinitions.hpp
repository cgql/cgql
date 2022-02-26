#ifndef TYPE_DEFINITIONS_HPP
#define TYPE_DEFINITIONS_HPP

#include "cgql/utilities/assert.h"
#include "cgql/utilities/cgqlDefs.h"
#include "cgql/base/cgqlPch.h"

namespace cgql {

using Int = int32_t;
using String = std::string_view;

namespace internal {

enum DefinitionType {
  INTERFACE_TYPE,
  OBJECT_TYPE,
  UNION_TYPE,
  TYPE_DEF,

  SCALAR_TYPE,

  LIST_TYPE,
  NON_NULL_TYPE
};

inline std::ostream& operator<<(std::ostream& os, const DefinitionType& type) {
  switch(type) {
    case OBJECT_TYPE:
      os << "OBJECT_TYPE"; break;
    case UNION_TYPE:
      os << "UNION_TYPE"; break;
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
  void setName(std::string name) {
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
};

class TypeDefinition : public AbstractTypeDefinition, public AbstractSchemaTypeDefinition {
public:
  TypeDefinition() {
    this->type = DefinitionType::TYPE_DEF;
  };
  TypeDefinition(const std::string& name) {
    this->setName(name);
  }
  ~TypeDefinition() {}
  const DefinitionType& getType() const override {
    return this->type;
  }
private:
  DefinitionType type;
};

template<typename T>
class ScalarTypeDefinition : public TypeDefinition {
public:
  ScalarTypeDefinition(
    const std::string& name
  ) {
    this->setName(name);
    this->type = DefinitionType::SCALAR_TYPE;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
private:
  DefinitionType type;
};

template<typename T>
class ListTypeDefinition : public TypeDefinition {
public:
  ListTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
    this->type = DefinitionType::LIST_TYPE;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  const DefinitionType& getType() const override {
    return this->type;
  }
private:
  mutable cgqlSPtr<T> innerType;
  DefinitionType type;
};

template<typename T>
class NonNullTypeDefinition : public TypeDefinition {
public:
  NonNullTypeDefinition(cgqlSPtr<T> innerType) {
    this->innerType = innerType;
    this->type = DefinitionType::NON_NULL_TYPE;
  }
  cgqlSPtr<T>& getInnerType() const {
    return this->innerType;
  };
  const DefinitionType& getType() const override {
    return this->type;
  }
private:
  mutable cgqlSPtr<T> innerType;
  DefinitionType type;
};

class ArgumentTypeDefinition : public AbstractSchemaTypeDefinition {
public:
  void setType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  cgqlSPtr<TypeDefinition>& getType() const {
    return this->type;
  }
private:
  mutable cgqlSPtr<TypeDefinition> type;
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
  InterfaceTypeDefinition() {
    this->type = DefinitionType::INTERFACE_TYPE;
  };
  void addField(const FieldTypeDefinition& field) {
    this->fields.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fields;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setImplementedInterfaces(cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fields;
  mutable cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> implements;
  DefinitionType type;
};

class ObjectTypeDefinition : public TypeDefinition {
public:
  ObjectTypeDefinition() {
    this->type = DefinitionType::OBJECT_TYPE;
  };
  void addField(const FieldTypeDefinition& field) {
    this->fieldDefs.emplace_back(field);
  }
  cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fieldDefs;
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
  void setImplementedInterfaces(cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> interfaces) {
    this->implements = interfaces;
  }
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  mutable cgqlContainer<FieldTypeDefinition> fieldDefs;
  mutable cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> implements;
  DefinitionType type;
};

class UnionTypeDefinition : public TypeDefinition {
public:
  UnionTypeDefinition() {
    type = DefinitionType::UNION_TYPE;
  }
  void addMember(const cgqlSPtr<TypeDefinition>& member) {
    this->members.emplace_back(member);
  }
  const DefinitionType& getType() const override {
    return this->type;
  }
private:
  DefinitionType type;
  mutable cgqlContainer<std::weak_ptr<TypeDefinition>> members;
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
      const cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>& implements = [this](const cgqlSPtr<TypeDefinition>& def) {
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
          default: return cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>>{};
        }
      }(def);
      for(auto const& interface : implements) {
        const auto& it = this->implementedInterfaces.find(interface->getName());
        if(it != this->implementedInterfaces.end()) {
          it->second.emplace_back(def);
        } else {
          cgqlContainer<cgqlSPtr<TypeDefinition>> typeDefVec;
          typeDefVec.reserve(1);
          typeDefVec.emplace_back(def);
          this->implementedInterfaces.try_emplace(
            interface->getName(), std::move(typeDefVec)
          );
        }
      }
    }
  }
  template<typename T>
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& getPossibleTypes(
    const T& abstractType
  ) const {
    const ImplementedInterfaces::const_iterator& it = this->implementedInterfaces.find(abstractType->getName());
    return it->second;
  }
private:
  cgqlSPtr<ObjectTypeDefinition> query;
  ImplementedInterfaces implementedInterfaces;
};

} // end of internal
} // end of cgql

#endif /* end of include guard: TYPE_DEFINITIONS_HPP */
