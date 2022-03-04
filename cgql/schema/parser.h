#pragma once

#include "cgql/base/baseParser.h"
#include "cgql/base/typeRegistry.h"
#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {
  
class SchemaParser : public BaseParser {
public:
  using BaseParser::BaseParser;

  void parse(const TypeRegistry& registry);
private:
  template<typename T = TypeDefinition>
  cgqlSPtr<T> parseType(const TypeRegistry& registry);
  void parseObjectTypeDefinition(const TypeRegistry& registry);
  void parseInterfaceTypeDefinition(const TypeRegistry& registry);
  void parseUnionTypeDefinition(const TypeRegistry& registry);
  void parseEnumTypeDefinition(const TypeRegistry& registry);
  void parseInputObjectTypeDefinition(const TypeRegistry& registry);
  FieldTypeDefinition parseFieldTypeDefinition(const TypeRegistry& registry);
  InputValueDefinition parseInputValueDefinition(const TypeRegistry& registry);
  ArgumentTypeDefinition parseArgumentDefinition(const TypeRegistry& registry);
  cgqlContainer<std::string> parseImplementInterfaces(const TypeRegistry& registry);

  void parseDefinition(const TypeRegistry& registry);
};

cgqlSPtr<internal::Schema> parseSchema(const char *source, const TypeRegistry& registry);

} /* internal */ 
} /* cgql */ 
