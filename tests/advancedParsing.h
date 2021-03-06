#include <cgql/cgql.h>
#include <cgql/parser/parser.h>
#include <cgql/logger/logger.h>
#include <fstream>

#include "utils/timer.hpp"

using namespace cgql;

inline void runAdvancedParsing() {{

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

  Timer timer("full execution");

  test.parseSchema(schema.c_str());
  ResolverMap root {
    {
      "person",
      [](const Args& arguments) -> Data {
        GraphQLInputTypes args = arguments["args"];
        Object a {
          {
            { "city", "0xFF-Park" },
            { "houseName", "cw3dv's-homeeeee" }
          }
        };
        Object r {
          {
            { "name", "cw3dv1" }
          }
        };
        List l {
          {
            "Google",
            "Microsoft",
            "Github"
          }
        };
        Object p {
          {
            { "name", fromVariant<std::string>(fromVariant<cgqlSPtr<ObjectType>>(args)->fields["name"]) },
            { "age", fromVariant<Int>(fromVariant<cgqlSPtr<ObjectType>>(args)->fields["id"]) },
            { "address", cgqlSMakePtr<Object>(a) },
            { "partner", cgqlSMakePtr<Object>(r) },
            { "gender", "MALE" },
            { "workedAt", cgqlSMakePtr<List>(l) }
          }
        };
        return cgqlSMakePtr<Object>(p);
      }
    },
    {
      "test",
      [](const Args& args) -> Data {
        Int i = fromVariant<Int>(args["arg"]);
        return 0;
      }
    }
  };
  TypeOfMap typeOfMap {
    {
      "Entity",
      [](const cgqlSPtr<Object>& result) -> String {
        return "Person";
      }
    }
  };
  for(int i = 0; i < 50000; i++) {
    auto doc = parse(
      "{"
      "  cw3dv: person(args: { name: \"bruhhh\" }) {"
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
    // printResultMap(*r.data);
    // for(auto err : r.errors) {
    //   logger::error(err.message);
    // }
  }
}}
