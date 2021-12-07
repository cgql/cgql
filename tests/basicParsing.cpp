#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>
#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/execute/execute.h"

int main() {
  GraphQLObject root {
    "Query",
    {
      {
        "name",
        GraphQLTypes::GraphQLString,
        []() -> String {
          return "cw3dv";
        }
      },
      {
        "age",
        GraphQLTypes::GraphQLInt,
        []() -> Int {
          return 14;
        }
      }
    }
  };

  GraphQLSchema schema {
    root
  };

  Document ast = parse(
    "{"
    "  name"
    "  age"
    "}"
  );

  printDocumentNode(ast);
  for(auto e : execute(schema, ast)) {
  }

  return 0;
}
