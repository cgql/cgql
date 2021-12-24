#include "Document.h"
#include "cgql/logger/logger.h"

namespace cgql {
namespace internal {

using std::ostream;

// Field
Field::Field(
  const string& name,
  const SelectionSet& selectionSet
): selectionSet(selectionSet) {
  this->setName(name);
}

Field::~Field() {}

// OperationDefinition
OperationDefinition::OperationDefinition(
  const OperationType& operationType,
  const SelectionSet& selectionSet
): operationType(operationType), selectionSet(selectionSet) {}

OperationDefinition::~OperationDefinition() {}

FieldDefinition::~FieldDefinition() {}

ObjectTypeDefinition::~ObjectTypeDefinition() {}

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

void printSelectionSet(const internal::SelectionSet selectionSet, int level) {
  for(auto s : selectionSet) {
    string v;
    for(int i = 0; i < level; i++) v += "  ";
    std::visit([&v](internal::Selection&& args) {
      if(std::holds_alternative<cgqlSPtr<internal::Field>>(args)) {
        v += std::get<cgqlSPtr<internal::Field>>(args)->getName();
      }
    }, s);
    logger::info(v);
    cgqlSPtr<internal::Field> selection = std::get<cgqlSPtr<internal::Field>>(s);
    if(!selection->getSelectionSet().empty()) {
      printSelectionSet(selection->getSelectionSet(), level + 1);
    }
  }
}

void printGQLObj(const internal::ObjectTypeDefinition& obj, int level) {
  string indentation;
  for(int i = 0; i < level; i++) indentation += " ";
  logger::info(obj.getName().data());
  for(auto field : obj.getFields()) {
    logger::info(indentation + field.getName().data());
  }
}

void printDocumentNode(const internal::Document &doc) {
  for(internal::Definition def : doc.getDefinitions()) {
    if(internal::OperationDefinition* opDef = std::get_if<internal::OperationDefinition>(&def)) {
      logger::info(opDef->getOperationType());
      printSelectionSet(opDef->getSelectionSet(), 0);
    } else if(internal::TypeDefinition* typeDef =
      std::get_if<internal::TypeDefinition>(&def)) {
      if(typeDef->index() == 0) {
        internal::ObjectTypeDefinition obj =
          std::get<internal::ObjectTypeDefinition>(*typeDef);
        printGQLObj(obj, 2);
      }
    }
  }
}

void printResultMap(const ResultMap& rm, uint8_t level) {
  string indentation;
  for(auto i = 0; i < level; i++) indentation += "  ";
  for(auto const& [key, value] : rm.data) {
    if(value.index() == 0) {
      auto rg = std::get<GraphQLReturnTypes>(value);
      if(rg.index() == 0) {
        string v = indentation +
          key.data() +
          " " + std::to_string(std::get<Int>(rg));
        logger::info(v);
      } else if(rg.index() == 1) {
        string v = indentation +
          key.data() +
          " " + std::get<String>(rg).data();
        logger::info(v);
      }
    } else {
      string v = indentation + key.data();
      logger::info(v);
      printResultMap(*std::get<std::shared_ptr<ResultMap>>(value), level + 1);
    }
  }
}

} // cgql
