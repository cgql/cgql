#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>
#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/execute/execute.h"

void printRM(const ResultMap& rm) {
  for(auto const& [key, value] : rm.data) {
    if(value.index() == 0) {
      auto rg = std::get<GraphQLReturnTypes>(value);
      if(rg.index() == 0) {
        logger::info(std::get<Int>(rg));
      } else if(rg.index() == 1) {
        logger::info(std::get<String>(rg));
      }
    } else {
      printRM(*std::get<shared_ptr<ResultMap>>(value));
    }
  }
}

int main() {
  GraphQLObject* person = new GraphQLObject(
    "Person",
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
  );
  GraphQLObject root {
    "Query",
    {
      {
        "person",
        GraphQLTypes::GraphQLObjectType,
        [&]() -> GraphQLObject* {
          return person;
        }
      }
    }
  };

  GraphQLSchema schema {
    root
  };

  Document ast = parse(
    "{"
    "  person {"
    "    name"
    "    age"
    "  }"
    "}"
  );

  printDocumentNode(ast);
  ResultMap r = execute(schema, ast);
  return 0;
}
