#pragma once

#include "cgql/base/cgqlPch.h"

namespace cgql {
namespace internal {

enum class DirectiveLocation {
  QUERY,
  MUTATION,
  SUBSCRIPTION,
  FIELD,
  FRAGMENT_DEFINITION,
  FRAGMENT_SPREAD,
  INLINE_FRAGMENT,
  VARIABLE_DEFINITION,

  SCHEMA,
  SCALAR,
  OBJECT,
  FIELD_DEFINITION,
  ARGUMENT_DEFINITION,
  INTERFACE,
  UNION,
  ENUM,
  ENUM_VALUE,
  INPUT_OBJECT,
  INPUT_FIELD_DEFINITION
};

static std::unordered_map<std::string_view, DirectiveLocation> possibleDirectiveLocations = {
  { "QUERY", DirectiveLocation::QUERY },
  { "MUTATION", DirectiveLocation::MUTATION },
  { "SUBSCRIPTION", DirectiveLocation::SUBSCRIPTION },
  { "FIELD", DirectiveLocation::FIELD },
  { "FRAGMENT_DEFINITION", DirectiveLocation::FRAGMENT_DEFINITION },
  { "FRAGMENT_SPREAD", DirectiveLocation::FRAGMENT_SPREAD },
  { "INLINE_FRAGMENT", DirectiveLocation::INLINE_FRAGMENT },
  { "VARIABLE_DEFINITION", DirectiveLocation::VARIABLE_DEFINITION },

  { "SCHEMA", DirectiveLocation::SCHEMA },
  { "SCALAR", DirectiveLocation::SCALAR },
  { "OBJECT", DirectiveLocation::OBJECT },
  { "FIELD_DEFINITION", DirectiveLocation::FIELD_DEFINITION },
  { "ARGUMENT_DEFINITION", DirectiveLocation::ARGUMENT_DEFINITION },
  { "INTERFACE", DirectiveLocation::INTERFACE },
  { "UNION", DirectiveLocation::UNION },
  { "ENUM", DirectiveLocation::ENUM },
  { "ENUM_VALUE", DirectiveLocation::ENUM_VALUE },
  { "INPUT_OBJECT", DirectiveLocation::INPUT_OBJECT },
  { "INPUT_FIELD_DEFINITION", DirectiveLocation::INPUT_FIELD_DEFINITION }
};

inline DirectiveLocation getDirectiveLocation(const std::string& str) {
  // the given string must be in the possibleDirectiveLocations... hence no error checking here!
  return possibleDirectiveLocations.at(str);
}

} // internal
} // cgql
