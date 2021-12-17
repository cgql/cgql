#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

using namespace cgql;

inline void runAdvancedParsing() {
  auto typedefs = parse(
    "type Address {"
    "  city: String"
    "  houseName: Int"
    "}"
    ""
    "type Person {"
    "  name: String"
    "  age: Int"
    "}"
    ""
    "type Query {"
    "  person: Person"
    "}"
  );
  printDocumentNode(typedefs);
}
