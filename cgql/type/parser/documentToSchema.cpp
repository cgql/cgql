#include "cgql/type/parser/documentToSchema.h"

namespace cgql {
  namespace internal {
    GraphQLScalarTypes DocToSchemaParser::buildType(
      const std::string& typeName,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      if(typeName == "String") {
        return GraphQLTypes::GraphQLString;
      } else if(typeName == "Int") {
        return GraphQLTypes::GraphQLInt;
      }
      auto it = typeMap.find(typeName);
      ObjectTypeDefinition type =
        fromVariant<ObjectTypeDefinition>(it->second);
      GraphQLObject builtType = buildObject(typeName, type, typeMap);
      return cgqlSMakePtr<GraphQLObject>(builtType);
    }
    void DocToSchemaParser::buildArguments(
      GraphQLField& field,
      const FieldDefinition& fieldDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      for(auto const& argDef : fieldDef.getArgs()) {
        GraphQLArgument arg;
        arg.setName(argDef.getName());
        arg.setType(this->buildType(argDef.getType(), typeMap));
        field.addArg(arg.getName(), arg);
      }
    }
    cgqlContainer<GraphQLField> DocToSchemaParser::buildFields(
      const ObjectTypeDefinition& objDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      cgqlContainer<GraphQLField> fields;
      fields.reserve(objDef.getFields().size());
      for(auto const& fieldDef : objDef.getFields()) {
        GraphQLField field;
        field.setName(fieldDef.getName());
        field.setType(this->buildType(fieldDef.getType(), typeMap));
        this->buildArguments(field, fieldDef, typeMap);
        fields.push_back(field);
      }
      return fields;
    }
    GraphQLObject DocToSchemaParser::buildObject(
      const std::string& name,
      const ObjectTypeDefinition& typeDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      GraphQLObject obj;
      obj.setName(name);
      obj.getMutableFields() = buildFields(typeDef, typeMap);
      this->typeNameCache.push_back(name);
      return obj;
    }
    GraphQLSchema DocToSchemaParser::docToSchemaImpl(
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      GraphQLSchema schema;
      this->typeNameCache.reserve(typeMap.size());
      for(auto const& [ key, value ] : typeMap) {
        if(value.index() == 0) {
          if(std::find(
            this->typeNameCache.begin(),
            this->typeNameCache.end(),
            key
          ) != this->typeNameCache.end()) continue;
          GraphQLObject obj;
          obj = buildObject(
            key,
            fromVariant<ObjectTypeDefinition>(value),
            typeMap
          );
          if(obj.getName() == "Query") {
            schema.setQuery(obj);
          }
        }
      }

      return schema;
    }
  }
}
