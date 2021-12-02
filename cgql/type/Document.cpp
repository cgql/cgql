#include "Document.h"

// Field
Field::Field(
  const string& name,
  const SelectionSet& selectionSet
): name(name), selectionSet(selectionSet) {}

Field::~Field() {}

// OperationDefinition
OperationDefinition::OperationDefinition(
  OperationType operationType,
  const SelectionSet& selectionSet
): operationType(operationType), selectionSet(selectionSet) {}

OperationDefinition::~OperationDefinition() {}

// Document
Document::Document(
  const vector<OperationDefinition>& definitions
): definitions(definitions) {}

Document::~Document() {}
