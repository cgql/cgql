#include <cgql/type/parser/parser.h>
#include <cgql/logger/logger.h>

int main() {
  Document ast = parse(
    "{"
    "  name"
    "  address {"
    "    houseName"
    "    city"
    "  }"
    "  age"
    "}"
  );

  printDocumentNode(ast);

  return 0;
}
