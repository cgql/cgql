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
    schema += line;
  }
  file.close();

  test.parseSchema(schema.c_str());
  ResolverMap root {
    {
      "character",
      [](const Args& args) -> Data {
        ResultMap human {
          {
            { "name", "cw3dv" },
            { "age", 14 },
            { "workedAt",
              cgqlContainer<GraphQLReturnTypes>{
                "Google",
                "Microsoft",
                "Github"
              }
            }
          }
        };
        return cgqlSMakePtr<ResultMap>(human);
      }
    },
  };
  TypeOfMap typeOfMap {
    {
      "Character",
      [](const cgqlSPtr<ResultMap>& result) -> String {
        return "Human";
      }
    }
  };
  for(int i = 0; i < 50000; i++) {
    auto doc = parse(
      "{"
      "  cw3dv: character {"
      "    name"
      "    ... on Human {"
      "      age"
      "      workedAt"
      "    }"
      "    ... on Pet {"
      "      breed"
      "    }"
      "  }"
      "}"
    );
    auto r = test.executeWith(doc, root, typeOfMap);
    // printResultMap(*r);
  }
}
