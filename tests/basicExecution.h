#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>
#include <cgql/execute/execute.h>

using namespace cgql;

inline void runBasicExecution() {
  for(int i = 0; i < 1; i++) {
    /* GraphQLObject address {
      "Address",
      {
        {
          "city",
          GraphQLTypes::GraphQLString
        },
        {
          "houseName",
          GraphQLTypes::GraphQLString
        }
      }
    };
    GraphQLObject person (
      "Person",
      {
        {
          "name",
          GraphQLTypes::GraphQLString
        },
        {
          "age",
          GraphQLTypes::GraphQLInt
        },
        {
          "address",
          cgqlSMakePtr<GraphQLObject>(address)
        }
      }
    );
    std::shared_ptr<GraphQLObject> person_ = std::make_shared<GraphQLObject>(person);
    GraphQLObject root {
      "Query",
      {
        {
          "person",
          person_,
          [&]() -> Data  {
            ResultMap a {
              {
                { "city", "0xFF-Park" },
                { "houseName", "cw3dv's-homeeeee" }
              }
            };
            ResultMap p {
              {
                { "name", "cw3dv" },
                { "age", 14 },
                { "address", cgqlSMakePtr<ResultMap>(a) }
              }
            };
            return std::make_shared<ResultMap>(p);
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
      "    age"
      "    address {"
      "      city"
      "      houseName"
      "    }"
      "  }"
      "}"
    );
    // printDocumentNode(ast);

    ResultMap r = execute(schema, ast);
    printResultMap(r); */
  }
}
