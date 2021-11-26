#include "utils/pch.h"
#include "schema/GraphQLDefinition.h"

int main() {
  GraphQLObject address {
    "Address",
      {
        {
          "houseName",
          GraphQLTypes::GraphQLString,
          [](unordered_map<string, GraphQLInputTypes>) -> String {
            return "";
          }
        }
      }
  };
  GraphQLObject person (
    "Person",
    {
      {
        "name",
        GraphQLTypes::GraphQLString,
        [](unordered_map<string, GraphQLInputTypes>) -> String {
          return "";
        }
      },
      {
        "address",
        &address,
        [](unordered_map<string, GraphQLInputTypes>) -> GraphQLObject* {
          return nullptr;
        }
      }
    }
  );
}
