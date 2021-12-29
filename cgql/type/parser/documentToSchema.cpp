#include "cgql/type/parser/documentToSchema.h"

namespace cgql {
  namespace internal {
    GraphQLScalarTypes DocToSchemaParser::buildType(
      const std::string& typeName,
      const std::unordered_map<std::string, TypeDefinition>& typeMap,
      const cgqlSPtr<GraphQLObject>& currObj
    ) {
      if(currObj->getName() == typeName) {
        return currObj;
      }
      if(typeName == "String") {
        return GraphQLTypes::GraphQLString;
      } else if(typeName == "Int") {
        return GraphQLTypes::GraphQLInt;
      }
      auto it = typeMap.find(typeName);
      ObjectTypeDefinition type =
        fromVariant<ObjectTypeDefinition>(it->second);
      return buildObject(
        typeName,
        type,
        typeMap
      );
    }
    void DocToSchemaParser::buildArguments(
      GraphQLField& field,
      const FieldDefinition& fieldDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap,
      const cgqlSPtr<GraphQLObject>& currObj
    ) {
      for(auto const& argDef : fieldDef.getArgs()) {
        GraphQLArgument arg;
        arg.setName(argDef.getName());
        arg.setType(this->buildType(argDef.getType(), typeMap, currObj));
        field.addArg(arg.getName(), arg);
      }
    }
    cgqlContainer<GraphQLField> DocToSchemaParser::buildFields(
      const ObjectTypeDefinition& objDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap,
      const cgqlSPtr<GraphQLObject>& currObj
    ) {
      cgqlContainer<GraphQLField> fields;
      fields.reserve(objDef.getFields().size());
      for(auto const& fieldDef : objDef.getFields()) {
        GraphQLField field;
        field.setName(fieldDef.getName());
        field.setType(this->buildType(fieldDef.getType(), typeMap, currObj));
        this->buildArguments(field, fieldDef, typeMap, currObj);
        fields.push_back(field);
      }
      return fields;
    }
    cgqlSPtr<GraphQLObject> DocToSchemaParser::buildObject(
      const std::string& name,
      const ObjectTypeDefinition& typeDef,
      const std::unordered_map<std::string, TypeDefinition>& typeMap
    ) {
      cgqlSPtr<GraphQLObject> obj = cgqlSMakePtr<GraphQLObject>();
      obj->setName(name);
      obj->getMutableFields() =
        buildFields(
          typeDef,
          typeMap,
          obj
        );
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
          obj = *buildObject(
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
