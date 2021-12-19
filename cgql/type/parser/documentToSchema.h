#ifndef DOCUMENT_TO_SCHEMA_H
#define DOCUMENT_TO_SCHEMA_H

#include "../../cgqlPch.h"

#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/type/Document.h"

namespace cgql {
  namespace internal {
    struct DocToSchemaParser {
      GraphQLScalarTypes buildType(const string& typeName);
      std::vector<GraphQLField> buildFields(
        const ObjectTypeDefinition& objDef
      );
      GraphQLObject buildObject(
        const string& name,
        const ObjectTypeDefinition& objDef
      );
      GraphQLSchema docToSchemaImpl(
        const std::unordered_map<std::string, TypeDefinition>& typeMap
      );
      // data
      std::unordered_map<std::string, TypeDefinition> typeMap;
    };
  }
}

#endif /* ifndef DOCUMENT_TO_SCHEMA_H */
