#include "logger/logger.h"
#include "schema/GraphQLDefinition.h"
#include "type/Document.h"

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
  
  Field name {
    "name"
  };

  Document query {
    { // vector
      { // operation definition
        QUERY,
        { // selection set
          &name
        }
      }
    }
  };

  logger::info("Hello, You are handsome anyways");
  logger::success("Yayyyyyy!!");
  logger::error("Ooops!");
}
