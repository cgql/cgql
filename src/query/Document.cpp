#include "cgql/query/Document.h"
#include "cgql/logger/logger.h"
#include "cgql/cgqlDefs.h"
#include "../utils.h"

namespace cgql {

// OperationDefinition
OperationDefinition::OperationDefinition(
   OperationType operationType,
   SelectionSet selectionSet
): operationType(operationType), selectionSet(selectionSet) {}

OperationDefinition::~OperationDefinition() {}

// Document
Document::Document(
  cgqlContainer<Definition> definitions
): definitions(std::move(definitions)) {}

Document::~Document() {}

std::ostream& operator<<(std::ostream& out, OperationType type) {
  std::string outStr;
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

void printSelectionSet(const SelectionSet selectionSet, int level) {
  for(auto const& s : selectionSet) {
    std::string v;
    for(int i = 0; i < level; i++) v += "  ";
    if(s->getSelectionType() == SelectionType::FIELD) {
      const cgqlSPtr<Field>& field =
        std::static_pointer_cast<Field>(s);
      v += field->getName();
    }
    logger::info(v);
    printSelectionSet(s->getSelectionSet(), level + 1);
  }
}

void printDocumentNode(const Document &doc) {
  for(const Definition& def : doc.getDefinitions()) {
    if(def.index() == 0)  {
      const OperationDefinition& opDef =
        fromVariant<OperationDefinition>(def);
      logger::info(opDef.getOperationType());
      printSelectionSet(opDef.getSelectionSet(), 0);
    }
  }
}

void printScalarValue(
  const std::string& key,
  const Data& value,
  const std::string& indentation
) {
  if(value.index() == 0) {
    std::string v = indentation +
      key.data() +
      " " + std::to_string(fromVariant<Int>(value));
    logger::info(v);
  } else if(value.index() == 1) {
    std::string v = indentation +
      key.data() +
      " " + fromVariant<String>(value).data();
    logger::info(v);
  }
}

void printList(
  const std::string& key,
  const cgqlSPtr<List>& value,
  const std::string& indentation
) {
  logger::info(indentation + key);
  for(auto const& each : value->elements) {
    printScalarValue("", each, indentation);
  }
}

void printResultMap(const Object& obj, uint8_t level) {
  std::string indentation;
  for(auto i = 0; i < level; i++) indentation += "  ";
  for(auto const& [key, value] : obj.fields) {
    if(isList(value)) {
      printList(key, fromVariant<cgqlSPtr<List>>(value), indentation);
    } else if(isObject(value)) {
      std::string v = indentation + key.data();
      logger::info(v);
      printResultMap(*fromVariant<std::shared_ptr<Object>>(value), level + 1);
    } else {
      printScalarValue(key, value, indentation);
    }
  }
}

} // cgql
