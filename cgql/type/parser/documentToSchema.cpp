#include "cgql/type/parser/documentToSchema.h"

namespace cgql {
  namespace internal {
    GraphQLScalarTypes DocToSchemaParser::buildType(
      const Type& type,
      const TypeMetaData& typeMetaData,
      const std::unordered_map<std::string, TypeDefinition>& typeMap,
      const cgqlSPtr<GraphQLObject>& currObj
    ) {
      if(typeMetaData.isList() || typeMetaData.isNonNull()) {
        cgqlAssert(!type.getWrappedInnerType().has_value(), "Inner type is empty");
        cgqlAssert(typeMetaData.getWrappedInnerType() == nullptr, "TypeMetaData of inner type is empty");
        return this->buildType(
          *type.getWrappedInnerType().value(),
          *typeMetaData.getWrappedInnerType(),
          typeMap,
          currObj
        );
      }
      std::string typeName = type.getName();
      if(currObj->getName() == typeName) {
        return currObj;
      }
      if(typeName == "String") {
        return GraphQLTypes::GraphQLString;
      } else if(typeName == "Int") {
        return GraphQLTypes::GraphQLInt;
      }
      auto it = typeMap.find(typeName);
      ObjectTypeDefinition objType =
        fromVariant<ObjectTypeDefinition>(it->second);
      return buildObject(
        typeName,
        objType,
        typeMap
      );
    }
    TypeMetaData DocToSchemaParser::buildTypeMetaData(
      const Type& type
    ) {
      TypeMetaData typeMetaData;
      bool hasInnerType = type.getWrappedInnerType().has_value();
      if(hasInnerType) {
        TypeMetaData innerType =
          buildTypeMetaData(*type.getWrappedInnerType().value());
        typeMetaData.setWrappedInnerType(cgqlSMakePtr<TypeMetaData>(innerType));
      }
      typeMetaData.setIsList(type.isList());
      typeMetaData.setIsNonNull(type.isNonNull());
      return typeMetaData;
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
        arg.setTypeMetaData(this->buildTypeMetaData(argDef.getType()));
        arg.setType(this->buildType(
          argDef.getType(),
          arg.getTypeMetaData(),
          typeMap,
          currObj
        ));
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
        field.setTypeMetaData(this->buildTypeMetaData(fieldDef.getType()));
        field.setType(this->buildType(
          fieldDef.getType(),
          field.getTypeMetaData(),
          typeMap,
          currObj
        ));
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
