#include <cgql/cgql.h>
#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>
#include <fstream>

using namespace cgql;

inline void runAdvancedParsing() {
  CgqlInstance test;
  std::string schema;
  std::ifstream file;
  file.open("tests/test.gql");
  std::string line;

  while(std::getline(file, line)) {
    line += "\n";
    schema += line;
  }
  file.close();

  test.parseSchema(schema.c_str());
  ResolverMap root {
    {
      "person",
      [](const Args& arguments) -> Data {
        auto args = fromVariant<InputObject>(arguments["args"]);
        ResultMap a {
          {
            { "city", "0xFF-Park" },
            { "houseName", "cw3dv's-homeeeee" }
          }
        };
        ResultMap r {
          {
            { "name", "cw3dv1" }
          }
        };
        ResultMap p {
          {
            { "name", fromVariant<std::string>(fromVariant<GraphQLInputTypes>(args->argsMap["name"])) },
            { "age", fromVariant<Int>(fromVariant<GraphQLInputTypes>(args->argsMap["id"])) },
            { "address", cgqlSMakePtr<ResultMap>(a) },
            { "partner", cgqlSMakePtr<ResultMap>(r) },
            { "gender", "MALE" },
            { "workedAt",
              (cgqlContainer<GraphQLReturnTypes>){
                "Google",
                "Microsoft",
                "Github"
              }
            }
          }
        };
        return cgqlSMakePtr<ResultMap>(p);
      }
    },
    {
      "test",
      [](const Args& args) -> Data {
        Int i = fromVariant<Int>(fromVariant<GraphQLInputTypes>(args["arg"]));
        return 0;
      }
    }
  };
  TypeOfMap typeOfMap {
    {
      "Entity",
      [](const cgqlSPtr<ResultMap>& result) -> String {
        return "Person";
      }
    }
  };
  for(int i = 0; i < 50000; i++) {
    auto doc = parse(
      "{"
      "  cw3dv: person(args: { id: 65 name: \"Hello\" } ) {"
      "    name"
      "    age"
      "    addr: address {"
      "      city"
      "      houseName"
      "    }"
      "    partner {"
      "      name"
      "    }"
      "    workedAt"
      "  }"
      "}"
    );
    auto r = test.executeWith(doc, root, typeOfMap);
    // printResultMap(*r);
  }
}
