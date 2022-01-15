#include <cgql/execute/execute.h>
#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

using namespace cgql;

inline void runAdvancedParsing() {
  auto typedefs = parseSchema(
    "type Address {"
    "  city: String"
    "  houseName: String"
    "}"
    ""
    "type Person {"
    "  name: String!"
    "  age: Int"
    "  address: Address"
    "  partner: Person"
    "  workedAt: [String]"
    "}"
    ""
    "type Query {"
    "  person(id: Int): Person"
    "}"
  );
  for(int i = 0; i < 50000; i++) {
    auto doc = parse(
      "{"
      "  cw3dv: person(id: 65) {"
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
    ResolverMap root {
      {
        "person",
        [](const Args& args) -> Data {
          Int id = fromVariant<Int>(args["id"]);
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
              { "name", "cw3dv" },
              { "age", id },
              { "address", cgqlSMakePtr<ResultMap>(a) },
              { "partner", cgqlSMakePtr<ResultMap>(r) },
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
    };
    auto r = execute(typedefs, doc, root);
    // printResultMap(r);
  }
}
