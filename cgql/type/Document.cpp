#include "Document.h"

using std::ostream;

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

ostream& operator<<(ostream& out, OperationType type) {
  string outStr;
  switch(type) {
    case OperationType::QUERY:
      outStr = "QUERY";
      break;
    case OperationType::MUTATION:
      outStr = "MUTATION";
      break;
    case OperationType::SUBSCRIPTION:
      outStr = "SUBSCRIPTION";
      break;
  }
  out << outStr;
  return out;
}

void printSelectionSet(SelectionSet selectionSet, int level) {
  for(auto s : selectionSet) {
    string v;
    for(int i = 0; i < level; i++) v += "  ";
    std::visit([&v](Selection&& args) {
      if(std::holds_alternative<Field*>(args)) {
        v += std::get<Field*>(args)->getName();
      }
    }, s);
    logger::info(v);
    Field* selection = std::get<Field*>(s);
    if(!selection->getSelectionSet().empty()) {
      printSelectionSet(selection->getSelectionSet(), level + 1);
    }
  }
}

void printDocumentNode(Document &doc) {
  for(OperationDefinition opDef : doc.getDefinitions()) {
    logger::info(opDef.getOperationType());
    printSelectionSet(opDef.getSelectionSet(), 0);
  }
}
