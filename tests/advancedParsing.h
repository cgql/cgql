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
      "firstSearchResult",
      [](const Args& args) -> Data {
        ResultMap human {
          {
            { "name", "cw3dv" },
            { "age", 14 }
          }
        };
        return cgqlSMakePtr<ResultMap>(human);
      }
    },
  };
  TypeOfMap typeOfMap {
    {
      "SearchResult",
      [](const cgqlSPtr<ResultMap>& result) -> String {
        return "Person";
      }
    }
  };
  for(int i = 0; i < 1; i++) {
    auto doc = parse(
      "{"
      "  cw3dv: firstSearchResult {"
      "    ... on Human {"
      "      name"
      "      ...HumanFragment"
      "    }"
      "    ... on Photo {"
      "      width"
      "    }"
      "  }"
      "}"
      ""
      "fragment HumanFragment on Human {"
      "  age"
      "}"
    );
    auto r = test.executeWith(doc, root, typeOfMap);
    // printResultMap(*r);
  }
}
