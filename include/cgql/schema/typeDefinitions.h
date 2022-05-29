#pragma once

#include "cgql/schema/GraphQLTypes.h"
#include "cgql/parser/directiveLocations.h"
#include "cgql/cgqlDefs.h"

#include <cassert>

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

template<typename Ostream>
inline Ostream& operator<<(Ostream& os, const DefinitionType& type) {
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
  void setName(std::string name);
  const std::string& getName() const;
  void setDescription(std::string description);
  const std::string& getDescription() const;
private:
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
  void addArgument(DirectiveArgument argument);
  const cgqlContainer<DirectiveArgument>& getArguments() const;
private:
  cgqlContainer<DirectiveArgument> args;
};

class TypeDefinitionWithDirectives {
public:
  void setDirectives(cgqlContainer<Directive> directives);
  const cgqlContainer<Directive>& getDirectives() const;
private:
  cgqlContainer<Directive> directives;
};

class TypeDefinition : public AbstractSchemaTypeDefinition {
public:
  virtual ~TypeDefinition();
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
  ScalarTypeDefinition();
  ScalarTypeDefinition(
    std::string name,
    Serializer serializer
  );
  DefinitionType getDefinitionType() const override {
    return DefinitionType::SCALAR;
  }
  Serializer serializer;
private:
};

class ListTypeDefinition : public TypeDefinition {
public:
  ListTypeDefinition(cgqlSPtr<TypeDefinition> innerType);
  const cgqlSPtr<TypeDefinition>& getInnerType() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::LIST;
  }
private:
  cgqlSPtr<TypeDefinition> innerType;
};

class NonNullTypeDefinition : public TypeDefinition {
public:
  NonNullTypeDefinition(cgqlSPtr<TypeDefinition> innerType);
  const cgqlSPtr<TypeDefinition>& getInnerType() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::NON_NULL;
  }
private:
  cgqlSPtr<TypeDefinition> innerType;
};

class DefaultWrapTypeDefinition : public TypeDefinition {
public:
  DefaultWrapTypeDefinition(cgqlSPtr<TypeDefinition> innerType);
  cgqlSPtr<TypeDefinition> getInnerType() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::DEFAULT_WRAP;
  }
private:
  std::weak_ptr<TypeDefinition> innerType;
};

class InputValueDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void setDefaultValue(GraphQLInputTypes value);
  const GraphQLInputTypes& getDefaultValue() const;
  void setInputValueType(cgqlSPtr<TypeDefinition> type);
  const cgqlSPtr<TypeDefinition>& getInputValueType() const;
private:
  cgqlSPtr<TypeDefinition> type;
  GraphQLInputTypes defaultValue;
};

class FieldTypeDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void setFieldType(cgqlSPtr<TypeDefinition> type);
  const cgqlSPtr<TypeDefinition>& getFieldType() const;
  void addArg(InputValueDefinition arg);
  const cgqlContainer<InputValueDefinition>& getArgs() const;
private:
  cgqlSPtr<TypeDefinition> type;
  cgqlContainer<InputValueDefinition> argDefs;
};

class InterfaceTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(FieldTypeDefinition field);
  const cgqlContainer<FieldTypeDefinition>& getFields() const;
  void addImplementedInterface(std::string interface);
  const cgqlContainer<std::string>& getImplementedInterfaces() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INTERFACE;
  }
private:
  cgqlContainer<FieldTypeDefinition> fields;
  cgqlContainer<std::string> implements;
};

class ObjectTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(FieldTypeDefinition field);
  const cgqlContainer<FieldTypeDefinition>& getFields() const;
  void addImplementedInterface(std::string interface);
  const cgqlContainer<std::string>& getImplementedInterfaces() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::OBJECT;
  }
private:
  cgqlContainer<FieldTypeDefinition> fieldDefs;
  cgqlContainer<std::string> implements;
};

class UnionTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addMember(cgqlSPtr<ObjectTypeDefinition> member);
  const cgqlContainer<cgqlSPtr<TypeDefinition>>& getMembers() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::UNION;
  }
private:
  cgqlContainer<cgqlSPtr<TypeDefinition>> members;
};

struct EnumValueDefinition :
  public AbstractSchemaTypeDefinition,
  public TypeDefinitionWithDirectives {
  EnumValueDefinition(
    std::string description,
    std::string name
  );
};

class EnumTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addValue(EnumValueDefinition value);
  const cgqlContainer<EnumValueDefinition>& getValues() const;
  String serialize(String outputValue);
  DefinitionType getDefinitionType() const override {
    return DefinitionType::ENUM;
  }
private:
  cgqlContainer<EnumValueDefinition> values;
};

class InputObjectTypeDefinition :
  public TypeDefinition,
  public TypeDefinitionWithDirectives {
public:
  void addField(InputValueDefinition field);
  const cgqlContainer<InputValueDefinition>& getFields() const;
  DefinitionType getDefinitionType() const override {
    return DefinitionType::INPUT_OBJECT;
  } 
private:
  cgqlContainer<InputValueDefinition> fields;
};

class DirectiveTypeDefinition : public TypeDefinition {
public:
  void addArgument(InputValueDefinition argument);
  const cgqlContainer<InputValueDefinition>& getArguments() const;
  void setDirectiveLocations(cgqlContainer<DirectiveLocation> locations);
  const cgqlContainer<DirectiveLocation>& getDirectiveLocations() const;
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
  void setQuery(cgqlSPtr<ObjectTypeDefinition> query);
  const cgqlSPtr<ObjectTypeDefinition>& getQuery() const;
  void setTypeDefMap(
    const std::unordered_map<std::string, cgqlSPtr<TypeDefinition>>& typeDefMap
  );
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
