#include <cgql/execute/execute.h>
#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

using namespace cgql;

inline void runAdvancedParsing() {
  for(int i = 0; i < 50000; i++) {
    auto typedefs = parseSchema(
      "type Address {"
      "  city: String"
      "  houseName: String"
      "}"
      ""
      "type Person {"
      "  name: String"
      "  age: Int"
      "  address: Address"
      "}"
      ""
      "type Query {"
      "  person(id: Int): Person"
      "  sum(x: Int, y: Int): Int"
      "}"
    );
    auto doc = parse(
      "{"
      "  cw3dv: person(id: 65) {"
      "    name"
      "    age"
      "    addr: address {"
      "      city"
      "      houseName"
      "    }"
      "  }"
      "  sum(x: 15, y: 10)"
      "}"
    );
    ResolverMap root {
      {
        "person",
        [](const Args args) -> Data {
          Int id = fromVariant<Int>(args["id"]);
          ResultMap a {
            {
              { "city", "0xFF-Park" },
              { "houseName", "cw3dv's-homeeeee" }
            }
          };
          ResultMap p {
            {
              { "name", "cw3dv" },
              { "age", id },
              { "address", cgqlSMakePtr<ResultMap>(a) }
            }
          };
          return std::make_shared<ResultMap>(p);
        }
      },
      {
        "sum",
        [](const Args args) -> Data {
          Int x = fromVariant<Int>(args["x"]);
          Int y = fromVariant<Int>(args["y"]);
          return x + y;
        }
      }
    };
    auto r = execute(typedefs, doc, root);
    // printResultMap(r);
  }
}
