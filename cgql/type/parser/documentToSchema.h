#ifndef DOCUMENT_TO_SCHEMA_H
#define DOCUMENT_TO_SCHEMA_H

#include "cgql/cgqlPch.h"

#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/type/Document.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
  namespace internal {
    struct DocToSchemaParser {
      GraphQLScalarTypes buildType(
        const Type& type,
        const TypeMetaData& typeMetaData,
        const std::unordered_map<std::string, TypeDefinition>& typeMap,
        const cgqlSPtr<GraphQLObject>& currObj
      );
      TypeMetaData buildTypeMetaData(
        const Type& type
      );
      void buildArguments(
        GraphQLField& field,
        const FieldDefinition& fieldDef,
        const std::unordered_map<std::string, TypeDefinition>& typeMap,
        const cgqlSPtr<GraphQLObject>& currObj
      );
      cgqlContainer<GraphQLField> buildFields(
        const ObjectTypeDefinition& objDef,
        const std::unordered_map<std::string, TypeDefinition>& typeMap,
        const cgqlSPtr<GraphQLObject>& currObj
      );
      cgqlSPtr<GraphQLObject> buildObject(
        const std::string& name,
        const ObjectTypeDefinition& objDef,
        const std::unordered_map<std::string, TypeDefinition>& typeMap
      );
      GraphQLSchema docToSchemaImpl(
        const std::unordered_map<std::string, TypeDefinition>& typeMap
      );
      // data
      cgqlContainer<std::string> typeNameCache;
    };
  }
}

#endif /* ifndef DOCUMENT_TO_SCHEMA_H */
