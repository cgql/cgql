#include <cgql/execute/execute.h>
#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

using namespace cgql;

inline void runAdvancedParsing() {
  for(int i = 0; i < 50000; i++) {
    auto typedefs = parseSchema(
      "type Address {"
      "  city: String"
      "  houseName: Int"
      "}"
      ""
      "type Person {"
      "  name: String"
      "  age: Int"
      "  address: Address"
      "}"
      ""
      "type University {"
      "  universityName: String"
      "  address: Address"
      "}"
      ""
      "type Query {"
      "  person: Person"
      "  university: University"
      "}"
    );
    auto doc = parse(
      "{"
      "  person {"
      "    name"
      "    age"
      "    address {"
      "      city"
      "      houseName"
      "    }"
      "  }"
      "  university {"
      "    address {"
      "      city"
      "    }"
      "  }"
      "}"
    );
    ResolverMap root {
      {
        "person",
        []() -> Data {
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
      },
      {
        "university",
        []() -> Data {
          ResultMap a {
            {
              { "city", "Washington" },
              { "houseName", "uni-homeeeee" }
            }
          };
          ResultMap u {
            {
              { "universityName", "Harvard" },
              { "address", cgqlSMakePtr<ResultMap>(a) }
            }
          };
          return cgqlSMakePtr<ResultMap>(u);
        }
      }
    };
    auto r = execute(typedefs, doc, root);
    // printResultMap(r);
  }
}
