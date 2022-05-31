#include "cgql/logger/logger.h"
#include <cgql/cgql.h>
#include <cgql/parser/parser.h>
#include <fstream>
#include "utils/timer.hpp"

using namespace cgql;

inline void parseGHSchema() {{
  CgqlInstance test;
  std::string schema;
  std::ifstream file;
  file.open("tests/schema.docs.graphql");
  std::string line;

  while(std::getline(file, line)) {
    line += "\n";
    schema += line;
  }
  file.close();

  Timer timer("github schema");
  test.parseSchema(schema.c_str());
  logger::info("parsing successfull");
}}
