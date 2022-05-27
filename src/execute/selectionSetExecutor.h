#pragma once

#include "cgql/schema/typeDefinitions.hpp"
#include "cgql/query/Document.h"
#include "cgql/execute/execute.h"
#include "cgql/cgqlDefs.h"

namespace cgql {

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

}
