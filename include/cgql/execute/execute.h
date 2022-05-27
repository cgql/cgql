#pragma once

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/query/Document.h"

#include "cgql/cgqlDefs.h"

namespace cgql {
namespace internal {

struct ExecutionContext {
  cgqlSPtr<Schema> schema;
  ResolverMap resolverMap;
  TypeOfMap typeOfMap;
  cgqlContainer<FragmentDefinition> fragments;
};

class SelectionSetExecutor {
public:
  SelectionSetExecutor(cgqlSPtr<ObjectTypeDefinition> obj)
    : obj(obj) {
  }
  SelectionSetExecutor(
    cgqlSPtr<ObjectTypeDefinition> obj,
    cgqlSPtr<Object> objectValue
  )
    : obj(obj), source(objectValue) {
  }
  cgqlSPtr<Object> execute(
    const ExecutionContext& ctx,
    const SelectionSet& selectionSet
  );
private:
  Data executeField(
    const ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<TypeDefinition>& fieldType,
    const SelectionSet& fields
  );
  Data completeValue(
    const ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<TypeDefinition>& fieldType,
    const SelectionSet& fields,
    const Data& result
  );
  Data completeList(
    const ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<ListTypeDefinition<TypeDefinition>>& fieldType,
    const SelectionSet& fields,
    const Data& result
  );
  Data completeAbstractType(
    const ExecutionContext& ctx,
    const cgqlSPtr<TypeDefinition>& fieldType,
    const SelectionSet& fields,
    const Data& result
  );
  Args buildArgumentMap(
    const cgqlSPtr<Selection>& selection,
    const FieldTypeDefinition& fieldType
  );
private:
  cgqlSPtr<ObjectTypeDefinition> obj;
  cgqlSPtr<Object> source;
};

FieldTypeDefinition findGraphQLFieldByName(
  const cgqlSPtr<ObjectTypeDefinition>& objectType,
  const std::string& fieldName
);

void collectFields(
  const ExecutionContext& ctx,
  const cgqlSPtr<TypeDefinition>& objectType,
  const SelectionSet &selectionSet,
  GroupedField& groupedField
);

void mergeSelectionSet(
  const cgqlContainer<cgqlSPtr<Field>>& fields,
  SelectionSet& mergedSelectionSet
);

cgqlSPtr<Object> executeQuery(
  const ExecutionContext& ctx,
  const OperationDefinition& query,
  const cgqlSPtr<Schema>& schema
);

const OperationDefinition& getOperation(
  const Document& document,
  OperationType operationName = OperationType::QUERY
);

} // internal 

cgqlSPtr<Object> execute(
  const cgqlSPtr<internal::Schema>& schema,
  const internal::Document& document,
  const ResolverMap& resolverMap,
  const TypeOfMap& typeOfMap
);

}
