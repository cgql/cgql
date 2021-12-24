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
      "type Query {"
      "  person: Person"
      "}"
    );
    auto doc = parse(
      "{"
      "  cw3dv: person {"
      "    name"
      "    age"
      "    addr: address {"
      "      city"
      "      houseName"
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
      }
    };
    auto r = execute(typedefs, doc, root);
    // printResultMap(r);
  }
}
