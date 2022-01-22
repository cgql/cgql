#include "cgql/type/Document.h"
#include "cgql/logger/logger.h"
#include "cgql/utilities/cgqlDefs.h"

namespace cgql {
namespace internal {

// Field
Field::~Field() {}

// OperationDefinition
OperationDefinition::OperationDefinition(
  const OperationType& operationType,
  const SelectionSet& selectionSet
): operationType(operationType), selectionSet(selectionSet) {}

OperationDefinition::~OperationDefinition() {}

// Document
Document::Document(
  const cgqlContainer<Definition>& definitions
): definitions(definitions) {}

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
} // internal

void printSelectionSet(const internal::SelectionSet selectionSet, int level) {
  for(auto s : selectionSet) {
    std::string v;
    for(int i = 0; i < level; i++) v += "  ";
    std::visit([&v](internal::Selection&& args) {
      if(std::holds_alternative<cgqlSPtr<internal::Field>>(args)) {
        v += fromVariant<cgqlSPtr<internal::Field>>(args)->getName();
      }
    }, s);
    logger::info(v);
    cgqlSPtr<internal::Field> selection = fromVariant<cgqlSPtr<internal::Field>>(s);
    if(!selection->getSelectionSet().empty()) {
      printSelectionSet(selection->getSelectionSet(), level + 1);
    }
  }
}

void printDocumentNode(const internal::Document &doc) {
  for(const internal::Definition& def : doc.getDefinitions()) {
    if(def.index() == 0) {
      internal::OperationDefinition opDef =
        fromVariant<internal::OperationDefinition>(def);
      logger::info(opDef.getOperationType());
      printSelectionSet(opDef.getSelectionSet(), 0);
    } else if(def.index() == 1) {
      cgqlSPtr<internal::TypeDefinition> typeDef =
        fromVariant<cgqlSPtr<internal::TypeDefinition>>(def);
      if(typeDef->getType() == internal::DefinitionType::OBJECT_TYPE) {
        cgqlSPtr<internal::ObjectTypeDefinition> objDef =
          std::static_pointer_cast<internal::ObjectTypeDefinition>(typeDef);
        // printGQLObj(objDef, 2);
      }
    }
  }
}

void printScalarValue(
  const std::string& key,
  const Data& value,
  const std::string& indentation
) {
  auto rg = fromVariant<GraphQLReturnTypes>(value);
  if(rg.index() == 0) {
    std::string v = indentation +
      key.data() +
      " " + std::to_string(fromVariant<Int>(rg));
    logger::info(v);
  } else if(rg.index() == 1) {
    std::string v = indentation +
      key.data() +
      " " + fromVariant<String>(rg).data();
    logger::info(v);
  }
}

template<typename T>
void printList(
  const std::string& key,
  const cgqlContainer<T>& value,
  const std::string& indentation
) {
  logger::info(indentation + key);
  for(auto const& each : value) {
    printScalarValue("", each, indentation);
  }
}

void printResultMap(const ResultMap& rm, uint8_t level) {
  std::string indentation;
  for(auto i = 0; i < level; i++) indentation += "  ";
  for(auto const& [key, value] : rm.data) {
    if(value.index() == 0) {
      printScalarValue(key, value, indentation);
    } else if(isList(value)) {
      switch(value.index()) {
        case 2:
          printList(key, fromVariant<cgqlContainer<GraphQLReturnTypes>>(value), indentation);
          break;
        case 3:
          printList(key, fromVariant<cgqlContainer<cgqlSPtr<ResultMap>>>(value), indentation);
          break;
      }
    } else {
      std::string v = indentation + key.data();
      logger::info(v);
      printResultMap(*fromVariant<std::shared_ptr<ResultMap>>(value), level + 1);
    }
  }
}

} // cgql
