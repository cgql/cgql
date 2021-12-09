#include "Document.h"

namespace cgql {
namespace internal {

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
  const vector<Definition>& definitions
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
} // internal

void printSelectionSet(internal::SelectionSet selectionSet, int level) {
  for(auto s : selectionSet) {
    internal::string v;
    for(int i = 0; i < level; i++) v += "  ";
    std::visit([&v](internal::Selection&& args) {
      if(std::holds_alternative<internal::Field*>(args)) {
        v += std::get<internal::Field*>(args)->getName();
      }
    }, s);
    logger::info(v);
    internal::Field* selection = std::get<internal::Field*>(s);
    if(!selection->getSelectionSet().empty()) {
      printSelectionSet(selection->getSelectionSet(), level + 1);
    }
  }
}

void printDocumentNode(internal::Document &doc) {
  for(internal::Definition def : doc.getDefinitions()) {
    if(internal::OperationDefinition* opDef = std::get_if<internal::OperationDefinition>(&def)) {
      logger::info(opDef->getOperationType());
      printSelectionSet(opDef->getSelectionSet(), 0);
    }
  }
}
} // cgql
