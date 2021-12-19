#ifndef DOCUMENT_TO_SCHEMA_H
#define DOCUMENT_TO_SCHEMA_H

#include "../../cgqlPch.h"

#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/type/Document.h"

namespace cgql {
  namespace internal {
    struct DocToSchemaParser {
      GraphQLScalarTypes buildType(
        const string& typeName,
        const std::unordered_map<string, TypeDefinition>& typeMap
      );
      std::vector<GraphQLField> buildFields(
        const ObjectTypeDefinition& objDef,
        const std::unordered_map<string, TypeDefinition>& typeMap
      );
      GraphQLObject buildObject(
        const string& name,
        const ObjectTypeDefinition& objDef,
        const std::unordered_map<string, TypeDefinition>& typeMap
      );
      GraphQLSchema docToSchemaImpl(
        const std::unordered_map<std::string, TypeDefinition>& typeMap
      );
      // data
      std::unordered_map<string, GraphQLObject> typeMapCache;
    };
  }
}

#endif /* ifndef DOCUMENT_TO_SCHEMA_H */
