#pragma once

#include "cgql/parser/baseParser.h"
#include "cgql/schema/typeRegistry.h"
#include "cgql/schema/typeDefinitions.h"
#include "cgql/cgqlDefs.h"

namespace cgql {
  
class SchemaParser : public BaseParser {
public:
  using BaseParser::BaseParser;

  void parse(const TypeRegistry& registry);
private:
  cgqlSPtr<TypeDefinition> parseType(const TypeRegistry& registry);
  void parseObjectTypeDefinition(const TypeRegistry& registry);
  void parseInterfaceTypeDefinition(const TypeRegistry& registry);
  void parseUnionTypeDefinition(const TypeRegistry& registry);
  void parseEnumTypeDefinition(const TypeRegistry& registry);
  void parseInputObjectTypeDefinition(const TypeRegistry& registry);
  void parseScalarTypeDefinition(const TypeRegistry& registry);
  void parseDirectiveTypeDefinition(const TypeRegistry& registry);
  FieldTypeDefinition parseFieldTypeDefinition(const TypeRegistry& registry);
  InputValueDefinition parseInputValueDefinition(const TypeRegistry& registry);
  template<typename T>
  void parseImplementInterfaces(cgqlSPtr<T>& objectOrInterface);
  cgqlContainer<Directive> parseDirectives();
  cgqlContainer<DirectiveLocation> parseDirectiveLocations();
  Directive::DirectiveArgument parseDirectiveArgument();

  void parseDefinition(const TypeRegistry& registry);

  std::string parseDescription();
};

cgqlSPtr<Schema> parseSDLSchema(const char *source, const TypeRegistry& registry);

} /* cgql */ 
