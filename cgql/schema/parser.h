#ifndef SCHEMA_PARSER_H
#define SCHEMA_PARSER_H

#include "cgql/base/baseParser.h"
#include "cgql/base/typeRegistry.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {
  
class SchemaParser : public BaseParser {
private:
  template<typename T = TypeDefinition>
  cgqlSPtr<T> parseType();
  cgqlUPtr<ObjectTypeDefinition> parseObjectTypeDefinition();
  cgqlUPtr<InterfaceTypeDefinition> parseInterfaceTypeDefinition();
  FieldTypeDefinition parseFieldTypeDefinition();
  ArgumentTypeDefinition parseArgumentDefinition();
  cgqlContainer<cgqlSPtr<InterfaceTypeDefinition>> parseImplementInterfaces();
};

cgqlSPtr<internal::Schema> parseSchema(const char *source, const TypeRegistry& registry);

} /* internal */ 
} /* cgql */ 

#endif /* end of include guard: SCHEMA_PARSER_H */
