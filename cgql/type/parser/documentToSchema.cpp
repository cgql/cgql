#include "documentToSchema.h"

namespace cgql {
  namespace internal {
    GraphQLScalarTypes DocToSchemaParser::buildType(
      const string& typeName
    ) {
      if(typeName == "String") {
        return GraphQLTypes::GraphQLString;
      } else if(typeName == "Int") {
        return GraphQLTypes::GraphQLInt;
      }
      auto it = this->typeMap.find(typeName);
      ObjectTypeDefinition type =
        std::get<ObjectTypeDefinition>(it->second);
      GraphQLObject builtType = buildObject(typeName, type);
      return cgqlSMakePtr<GraphQLObject>(builtType);
    }
    std::vector<GraphQLField> DocToSchemaParser::buildFields(
      const ObjectTypeDefinition& objDef
    ) {
      std::vector<GraphQLField> fields;
      fields.reserve(objDef.getFields().size());
      for(FieldDefinition fieldDef : objDef.getFields()) {
        GraphQLField field;
        field.setName(fieldDef.getName());
        field.setType(this->buildType(fieldDef.getType()));
        fields.push_back(field);
      }
      return fields;
    }
    GraphQLObject DocToSchemaParser::buildObject(
      const string& name,
      const ObjectTypeDefinition& typeDef
    ) {
      GraphQLObject obj;
      obj.setName(name);
      obj.getMutableFields() = buildFields(typeDef);
      return obj;
    }
    GraphQLSchema DocToSchemaParser::docToSchemaImpl(
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      this->typeMap = typeMap;
      GraphQLSchema schema;
      for(auto const& [ key, value ] : typeMap) {
        if(value.index() == 0) {
          GraphQLObject obj;
          obj = buildObject(
            key,
            std::get<ObjectTypeDefinition>(value)
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
