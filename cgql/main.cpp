#include "utils/pch.h"
#include "schema/GraphQLDefinition.h"

int main() {
  GraphQLObject address (
    "Address",
    {
      {
        "houseName",
        GraphQLTypes::GraphQLString
      },
      {
        "city",
        GraphQLTypes::GraphQLString
      }
    }
  );
  GraphQLObject person (
    "Person",
    {
      {
        "name",
        GraphQLTypes::GraphQLString
      },
      {
        "address",
        &address
      }
    }
  );
}
