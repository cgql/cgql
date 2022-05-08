#include <cgql/cgql.h>
#include <cgql/type/parser/parser.h>
#include <fstream>

using namespace cgql;

inline void parseGHSchema() {
  CgqlInstance test;
  std::string schema;
  std::ifstream file;
  file.open("tests/github-schema.graphql");
  std::string line;

  while(std::getline(file, line)) {
    line += "\n";
    schema += line;
  }
  file.close();

  test.parseSchema(schema.c_str());
}
