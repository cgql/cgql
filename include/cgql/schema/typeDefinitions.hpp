#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/parser/directiveLocations.h"
#include "cgql/cgqlDefs.h"

#include <cassert>
#include <iostream>

namespace cgql {

enum class DefinitionType {
  TYPE_DEFINITION, // default base type

  DEFAULT_WRAP, // wraps field type including types in lists and non-nulls
  LIST,

  // type system locations based on spec
  SCALAR,
  OBJECT,
  INTERFACE,
  UNION,
  ENUM,
  INPUT_OBJECT,

  DIRECTIVE,
  NON_NULL
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
    case DefinitionType::DIRECTIVE:
      os << "DIRECTIVE_DEFINITION"; break;

    case DefinitionType::SCALAR:
      os << "SCALAR"; break;
    case DefinitionType::OBJECT:
      os << "OBJECT"; break;
    case DefinitionType::INTERFACE:
      os << "INTERFACE"; break;
    case DefinitionType::UNION:
      os << "UNION"; break;
    case DefinitionType::ENUM:
      os << "ENUM"; break;
    case DefinitionType::INPUT_OBJECT:
      os << "INPUT_OBJECT"; break;
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

class Directive : public AbstractSchemaTypeDefinition {
public:
  struct DirectiveArgument {
    std::string name;
    GraphQLInputTypes value;
  };
public:
  void addArgument(DirectiveArgument argument) {
    this->args.push_back(argument);
  }
  const cgqlContainer<DirectiveArgument>& getArguments() const {
    return this->args;
  }
private:
  cgqlContainer<DirectiveArgument> args;
};

class TypeDefinitionWithDirectives {
public:
  void setDirectives(cgqlContainer<Directive> directives) {
    this->directives = directives;
  }
  const cgqlContainer<Directive>& getDirectives() const {
    return this->directives;
  }
private:
  cgqlContainer<Directive> directives;
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

using Serializer = const Data&(*)(const Data&);

class ScalarTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  ScalarTypeDefinition() = default;
  ScalarTypeDefinition(
    const std::string& name,
    Serializer serializer
  ) : serializer(serializer) {
    this->setName(name);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::SCALAR;
  }
  Serializer serializer;
private:
};

template<typename T>
class ListTypeDefinition : public TypeDefinition {
public:
  ListTypeDefinition(cgqlSPtr<T> innerType)
    : innerType(innerType) {
  }
  const cgqlSPtr<T>& getInnerType() const {
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
  NonNullTypeDefinition(cgqlSPtr<T> innerType)
    : innerType(innerType) {
  }
  const cgqlSPtr<T>& getInnerType() const {
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
  DefaultWrapTypeDefinition(cgqlSPtr<T> innerType) 
    : innerType(innerType) {
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

class InputValueDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void setDefaultValue(GraphQLInputTypes value) {
    this->defaultValue = value;
  }
  const GraphQLInputTypes& getDefaultValue() const {
    return this->defaultValue;
  }
  void setInputValueType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  const cgqlSPtr<TypeDefinition>& getInputValueType() const {
    return this->type;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  GraphQLInputTypes defaultValue;
};

class FieldTypeDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void setFieldType(cgqlSPtr<TypeDefinition> type) {
    this->type = type;
  }
  const cgqlSPtr<TypeDefinition>& getFieldType() const {
    return this->type;
  }
  void addArg(InputValueDefinition arg) {
    this->argDefs.push_back(arg);
  }
  const cgqlContainer<InputValueDefinition>& getArgs() const {
    return this->argDefs;
  }
private:
  cgqlSPtr<TypeDefinition> type;
  cgqlContainer<InputValueDefinition> argDefs;
};

class InterfaceTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(FieldTypeDefinition field) {
    this->fields.push_back(field);
  }
  const cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fields;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INTERFACE;
  }
  void addImplementedInterface(std::string interface) {
    this->implements.push_back(interface);
  }
  const cgqlContainer<std::string>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  cgqlContainer<FieldTypeDefinition> fields;
  cgqlContainer<std::string> implements;
};

class ObjectTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(FieldTypeDefinition field) {
    this->fieldDefs.push_back(field);
  }
  const cgqlContainer<FieldTypeDefinition>& getFields() const {
    return this->fieldDefs;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::OBJECT;
  }
  void addImplementedInterface(std::string interface) {
    this->implements.push_back(interface);
  }
  const cgqlContainer<std::string>& getImplementedInterfaces() const {
    return this->implements;
  }
private:
  cgqlContainer<FieldTypeDefinition> fieldDefs;
  cgqlContainer<std::string> implements;
};

class UnionTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addMember(cgqlSPtr<ObjectTypeDefinition> member) {
    this->members.push_back(member);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::UNION;
  }
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& getMembers() const {
    return members;
  }
private:
  cgqlContainer<cgqlSPtr<TypeDefinition>> members;
};

struct EnumValueDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
  EnumValueDefinition(
    const std::string& description,
    const std::string& name
  ) {
    this->setDescription(description);
    this->setName(name);
  }
};

class EnumTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addValue(EnumValueDefinition value) {
    values.push_back(value);
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::ENUM;
  }
  const cgqlContainer<EnumValueDefinition>& getValues() const {
    return this->values;
  }
  String serialize(String outputValue) {
    cgqlContainer<EnumValueDefinition>::const_iterator it =
      std::find_if(
        values.begin(),
        values.end(),
        [&](const EnumValueDefinition& value) {
          return outputValue == value.getName();
        }
      );
    if(it != values.end())
      return outputValue;
    throw outputValue;
  }
private:
  cgqlContainer<EnumValueDefinition> values;
};

class InputObjectTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(InputValueDefinition field) {
    fields.push_back(field);
  }
  const cgqlContainer<InputValueDefinition>& getFields() const {
    return this->fields;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INPUT_OBJECT;
  } 
private:
  cgqlContainer<InputValueDefinition> fields;
};

class DirectiveTypeDefinition : public TypeDefinition {
public:
  void addArgument(InputValueDefinition argument) {
    arguments.push_back(argument);
  }
  const cgqlContainer<InputValueDefinition>& getArguments() const {
    return this->arguments;
  }
  void setDirectiveLocations(cgqlContainer<DirectiveLocation> locations) {
    this->locations = locations;
  }
  const cgqlContainer<DirectiveLocation>& getDirectiveLocations() const {
    return this->locations;
  }
  DefinitionType getDefinitionType() const override {
    return DefinitionType::DIRECTIVE;
  } 
private:
  cgqlContainer<InputValueDefinition> arguments;
  cgqlContainer<DirectiveLocation> locations;
  bool repeatable = false; // TODO
};

using ImplementedInterfaces = std::map<
  std::string,
  cgqlContainer<cgqlSPtr<TypeDefinition>>
>;

class Schema {
public:
  void setQuery(cgqlSPtr<ObjectTypeDefinition> query) {
    this->query = query;
  }
  const cgqlSPtr<ObjectTypeDefinition>& getQuery() const {
    return this->query;
  }
  void setTypeDefMap(
    const std::unordered_map<std::string, cgqlSPtr<TypeDefinition>>& typeDefMap
  ) {
    for(auto const& [key, def] : typeDefMap) {
      cgqlContainer<std::string> implements;
      switch(def->getDefinitionType()) {
        case DefinitionType::OBJECT: {
          cgqlSPtr<ObjectTypeDefinition> object =
            std::static_pointer_cast<ObjectTypeDefinition>(def);
          if(object->getName() == "Query") {
            this->setQuery(object);
          }
          implements = object->getImplementedInterfaces();
          break;
        }
        case DefinitionType::INTERFACE: {
          cgqlSPtr<InterfaceTypeDefinition> interface =
            std::static_pointer_cast<InterfaceTypeDefinition>(def);
          implements = interface->getImplementedInterfaces();
          break;
        }
        case DefinitionType::TYPE_DEFINITION:
          assert(false && "Type is empty");
          break;
        default: continue;
      }
      for(auto const& interface : implements) {
        this->implementedInterfaces[interface]
          .push_back(def);
      }
    }
  }
  template<typename T>
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& getPossibleTypes(
    const T& abstractType
  ) const {
    if(abstractType->getDefinitionType() == DefinitionType::UNION) {
      cgqlSPtr<UnionTypeDefinition> unionType =
        std::static_pointer_cast<UnionTypeDefinition>(abstractType);
      return unionType->getMembers();
    }
    ImplementedInterfaces::const_iterator it = this->implementedInterfaces.find(abstractType->getName());
    return it->second;
  }
private:
  ImplementedInterfaces implementedInterfaces;
  cgqlSPtr<ObjectTypeDefinition> query;
};

} // end of cgql
