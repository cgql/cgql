#include "type/parser/parser.h"
#include "logger/logger.h"

int main() {
//  GraphQLObject address {
//    "Address",
//      {
//        {
//          "houseName",
//          GraphQLTypes::GraphQLString,
//          [](unordered_map<string, GraphQLInputTypes>) -> String {
//            return "";
//          }
//        }
//      }
//  };
//  GraphQLObject person (
//    "Person",
//    {
//      {
//        "name",
//        GraphQLTypes::GraphQLString,
//        [](unordered_map<string, GraphQLInputTypes>) -> String {
//          return "";
//        }
//      },
//      {
//        "address",
//        &address,
//        [](unordered_map<string, GraphQLInputTypes>) -> GraphQLObject* {
//          return nullptr;
//        }
//      }
//    }
//  );
  parse(
    "{"
    "  name"
    "  address {"
    "    city"
    "  }"
    "  age"
    "  parents {"
    "    father"
    "    mother"
    "  }"
    "}"
  );
  parse("{}");
}
