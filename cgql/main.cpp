#include "type/parser/parser.h"
#include "logger/logger.h"
#include <variant>

void printSelectionSet(SelectionSet ss, int level) {
  for(auto s : ss) {
    string v;
    for(int i = 0; i < level; i++) v += "  ";
    std::visit([&v](Selection&& args) {
      if(std::holds_alternative<Field*>(args)) {
        v += std::get<Field*>(args)->getName();
      }
    }, s);
    logger::info(v);
    Field* selection = std::get<Field*>(s);
    if(!selection->getSelectionSet().empty()) {
      printSelectionSet(selection->getSelectionSet(), level + 1);
    }
  }
}

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
  Document ast = parse(
    "{"
    "  name"
    "  address {"
    "    houseName"
    "  }"
    "  age"
    "  parents {"
    "    father {"
    "      name"
    "      age"
    "    }"
    "    mother {"
    "      name"
    "      age"
    "    }"
    "  }"
    "}"
  );
  printSelectionSet(ast.getDefinitions()[0].getSelectionSet(), 0);
}
