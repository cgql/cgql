#include "utils/pch.h"
#include "schema/GraphQLDefinition.h"

int main() {
  GraphQLObject address {
    "Address",
      {
        {
          "houseName",
          GraphQLTypes::GraphQLString,
          [](unordered_map<string, GraphQLInputTypes>) -> GraphQLOutputTypes {
            return nullptr;
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
        [](unordered_map<string, GraphQLInputTypes>) -> GraphQLOutputTypes {
          return nullptr;
        }
      },
      {
        "address",
        &address,
        [](unordered_map<string, GraphQLInputTypes>) -> GraphQLOutputTypes {
          return nullptr;
        }
      }
    }
  );
}
