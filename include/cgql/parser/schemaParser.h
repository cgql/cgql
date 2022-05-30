#pragma once

#include "cgql/parser/baseParser.h"
#include "cgql/schema/typeRegistry.h"
#include "cgql/schema/typeDefinitions.h"
#include "cgql/cgqlDefs.h"

namespace cgql {
  
class SchemaParser : public BaseParser {
public:
  using BaseParser::BaseParser;

  void parse(TypeRegistry& registry);
private:
  cgqlSPtr<TypeDefinition> parseType(TypeRegistry& registry);
  void parseObjectTypeDefinition(TypeRegistry& registry);
  void parseInterfaceTypeDefinition(TypeRegistry& registry);
  void parseUnionTypeDefinition(TypeRegistry& registry);
  void parseEnumTypeDefinition(TypeRegistry& registry);
  void parseInputObjectTypeDefinition(TypeRegistry& registry);
  void parseScalarTypeDefinition(TypeRegistry& registry);
  void parseDirectiveTypeDefinition(TypeRegistry& registry);
  FieldTypeDefinition parseFieldTypeDefinition(TypeRegistry& registry);
  InputValueDefinition parseInputValueDefinition(TypeRegistry& registry);
  template<typename T>
  void parseImplementInterfaces(cgqlSPtr<T>& objectOrInterface);
  cgqlContainer<Directive> parseDirectives();
  cgqlContainer<DirectiveLocation> parseDirectiveLocations();
  Directive::DirectiveArgument parseDirectiveArgument();

  void parseDefinition(TypeRegistry& registry);

  std::string parseDescription();
};

cgqlSPtr<Schema> parseSDLSchema(const char *source, TypeRegistry& registry);

} /* cgql */ 
