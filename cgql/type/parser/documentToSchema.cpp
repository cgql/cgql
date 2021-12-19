#include "documentToSchema.h"

namespace cgql {
  namespace internal {
    GraphQLScalarTypes DocToSchemaParser::buildType(
      const string& typeName,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      if(typeName == "String") {
        return GraphQLTypes::GraphQLString;
      } else if(typeName == "Int") {
        return GraphQLTypes::GraphQLInt;
      }
      auto it = typeMap.find(typeName);
      ObjectTypeDefinition type =
        std::get<ObjectTypeDefinition>(it->second);
      GraphQLObject builtType = buildObject(typeName, type, typeMap);
      return cgqlSMakePtr<GraphQLObject>(builtType);
    }
    std::vector<GraphQLField> DocToSchemaParser::buildFields(
      const ObjectTypeDefinition& objDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      std::vector<GraphQLField> fields;
      fields.reserve(objDef.getFields().size());
      for(FieldDefinition fieldDef : objDef.getFields()) {
        GraphQLField field;
        field.setName(fieldDef.getName());
        field.setType(this->buildType(fieldDef.getType(), typeMap));
        fields.push_back(field);
      }
      return fields;
    }
    GraphQLObject DocToSchemaParser::buildObject(
      const string& name,
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
            std::get<ObjectTypeDefinition>(value),
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
