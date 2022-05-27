#pragma once

#include "cgql/schema/typeDefinitions.hpp"

namespace cgql::internal {

template<typename T>
struct hasSetImplInterfaces : std::false_type {};

template<>
struct hasSetImplInterfaces<ObjectTypeDefinition> : std::true_type {};

template<>
struct hasSetImplInterfaces<InterfaceTypeDefinition> : std::true_type {};

}
