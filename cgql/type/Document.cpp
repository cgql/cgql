#include "Document.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

using std::ostream;

// Field
Field::Field(
  string name,
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
    string v;
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

void printGQLObj(const GraphQLObject& obj, int level) {
  string indentation;
  for(int i = 0; i < level; i++) indentation += " ";
  logger::info(indentation + obj.getName().data());
  for(auto field : obj.getFields()) {
    logger::info(indentation + field.getName().data());
  }
}

void printDocumentNode(internal::Document &doc) {
  for(internal::Definition def : doc.getDefinitions()) {
    if(internal::OperationDefinition* opDef = std::get_if<internal::OperationDefinition>(&def)) {
      logger::info(opDef->getOperationType());
      printSelectionSet(opDef->getSelectionSet(), 0);
    } else if(internal::TypeDefinition* typeDef =
      std::get_if<internal::TypeDefinition>(&def)) {
      if(typeDef->index() == 0) {
        GraphQLObject obj =
          std::get<GraphQLObject>(*typeDef);
        printGQLObj(obj, 0);
      }
    }
  }
}
} // cgql
