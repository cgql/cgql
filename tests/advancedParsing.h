#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

using namespace cgql;

inline void runAdvancedParsing() {
  auto typedefs = parse(
    "type Person {"
    "  name: String"
    "  age: Int"
    "}"
  );
  printDocumentNode(typedefs);
}
