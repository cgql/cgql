#pragma once

#include "cgql/error/error.h"
#include "cgql/schema/typeDefinitions.h"
#include "cgql/query/Document.h"
#include "cgql/execute/execute.h"
#include "cgql/cgqlDefs.h"

namespace cgql {

struct ExecutionContext {
  cgqlSPtr<Schema> schema;
  ResolverMap resolverMap;
  TypeOfMap typeOfMap;
  cgqlContainer<FragmentDefinition> fragments;
  ErrorManager errorManager;
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
    ExecutionContext& ctx,
    const SelectionSet& selectionSet
  );
private:
  Data executeField(
    ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<TypeDefinition>& fieldType,
    const SelectionSet& fields
  );
  Data completeValue(
    ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<TypeDefinition>& fieldType,
    const SelectionSet& fields,
    const Data& result
  );
  Data completeList(
    ExecutionContext& ctx,
    const FieldTypeDefinition& field,
    const cgqlSPtr<ListTypeDefinition>& fieldType,
    const SelectionSet& fields,
    const Data& result
  );
  Data completeAbstractType(
    ExecutionContext& ctx,
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

}
