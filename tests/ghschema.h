#include "cgql/logger/logger.h"
#include <cgql/cgql.h>
#include <cgql/type/parser/parser.h>
#include <fstream>
#include <limits>

using namespace cgql;

inline void parseGHSchema() {
  logger::info("parsing started");
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
  logger::info("parsing successfull");
}
