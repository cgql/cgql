#include <cgql/cgql.h>
#include <cgql/type/parser/parser.h>

using namespace cgql;

inline void runBasicTest() {
  // configuration part
  CgqlInstance test;
  // schema
  test.parseSchema(
    "type Person {"
    "  name: String"
    "  age: Int"
    "}"
    ""
    "type Query {"
    "  person(id: Int): Person"
    "}"
  );

  // resolvers
  ResolverMap resolvers {
    {
      "person",
      [](const Args& args) -> Data {
        Int id = fromVariant<Int>(args["id"]); // argument
        Object p {
          {
            { "name", "cw3dv" },
            { "age", id }
          }
        };
        return cgqlSMakePtr<Object>(p);
      }
    }
  };
  // TypeOfMap (used for abstract types)
  TypeOfMap typeOfMap;
  // query
  auto query = parse(
    "{"
    "  cw3dv: person(id: 15) {"
    "    name"
    "    age"
    "  }"
    "}"
  );

  // execution
  auto executionResult = test.executeWith(query, resolvers, typeOfMap);
  printResultMap(*executionResult);
  /* prints result to stdout

  cw3dv
    name cw3dv
    age 14

  */ 
}
