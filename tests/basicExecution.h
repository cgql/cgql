#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>
#include "cgql/schema/GraphQLDefinition.h"
#include "cgql/execute/execute.h"

using namespace cgql;

inline void printRM(const ResultMap& rm, int level) {
  string indentation;
  for(auto i = 0; i < level; i++) indentation += "  ";
  for(auto const& [key, value] : rm.data) {
    if(value.index() == 0) {
      auto rg = std::get<GraphQLReturnTypes>(value);
      if(rg.index() == 0) {
        string v = indentation +
          key +
          " " + std::to_string(std::get<Int>(rg));
        logger::info(v);
      } else if(rg.index() == 1) {
        string v = indentation +
          key +
          " " + std::get<String>(rg);
        logger::info(v);
      }
    } else {
      string v = indentation + key;
      logger::info(v);
      printRM(*std::get<shared_ptr<ResultMap>>(value), level + 1);
    }
  }
}

inline void runBasicExecution() {
  GraphQLObject person (
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
      },
      {
        "address",
        GraphQLTypes::GraphQLObjectType,
        []() -> shared_ptr<GraphQLObject> {
          GraphQLObject a (
            "Address",
            {
              {
                "place",
                GraphQLTypes::GraphQLString,
                []() -> String {
                  return "World";
                }
              }
            }
          );
          return std::make_shared<GraphQLObject>(a);
        }
      }
    }
  );
  shared_ptr<GraphQLObject> person_ = std::make_shared<GraphQLObject>(person);
  GraphQLObject root {
    "Query",
    {
      {
        "person",
        GraphQLTypes::GraphQLObjectType,
        [&]() -> shared_ptr<GraphQLObject> {
          return person_;
        }
      }
    }
  };

  GraphQLSchema schema {
    root
  };

  auto ast = parse(
    "{"
    "  person {"
    "    name"
    "    address {"
    "      place"
    "    }"
    "  }"
    "  person"
    "}"
  );

  ResultMap r = execute(schema, ast);
  printRM(r, 0);
}