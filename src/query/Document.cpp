#include "cgql/query/Document.h"
#include "cgql/logger/logger.h"
#include "cgql/cgqlDefs.h"

namespace cgql {

void Argument::setValue(GraphQLInputTypes value) {
  this->value = value;
}
const GraphQLInputTypes& Argument::getValue() const {
  return this->value;
}

Selection::~Selection() = default;
void Selection::setSelectionSet(SelectionSet selectionSet) {
  this->selectionSet = std::move(selectionSet);
}
const SelectionSet& Selection::getSelectionSet() const {
  return this->selectionSet;
}
void Selection::setSelectionType(SelectionType type) {
  this->type = type;
}
SelectionType Selection::getSelectionType() const {
  return this->type;
}

Field::Field() {
  this->setSelectionType(SelectionType::FIELD);
};
void Field::setAlias(std::string alias) {
  this->alias = alias;
}
const std::string& Field::getAlias() const {
  return this->alias;
}
void Field::addArgs(Argument arg) {
  this->args.push_back(arg);
}
const cgqlContainer<Argument>& Field::getArgs() const {
  return this->args;
}
const std::string& Field::getResponseKey() const {
  return this->alias.empty() ? this->getName() : this->alias;
}

InlineFragment::InlineFragment() {
  this->setSelectionType(SelectionType::INLINE_FRAGMENT);
};
void InlineFragment::setTypeCondition(std::string typeCondition) {
  this->typeCondition = typeCondition;
}
const std::string& InlineFragment::getTypeCondition() const {
  return this->typeCondition;
}

Fragment::Fragment() {
  this->setSelectionType(SelectionType::FRAGMENT);
}

// OperationDefinition
OperationDefinition::OperationDefinition(
   SelectionSet selectionSet,
   OperationType operationType
): selectionSet(selectionSet), operationType(operationType) {}
OperationDefinition::OperationDefinition() = default;
const SelectionSet& OperationDefinition::getSelectionSet() const {
  return this->selectionSet; 
}
OperationType OperationDefinition::getOperationType() const {
  return this->operationType;
}

void FragmentDefinition::setTypeCondition(std::string typeCondition) {
  this->typeCondition = typeCondition;
}
const std::string& FragmentDefinition::getTypeCondition() const {
  return this->typeCondition;
}
void FragmentDefinition::setSelectionSet(SelectionSet selectionSet) {
  this->selectionSet = std::move(selectionSet);
}
const SelectionSet& FragmentDefinition::getSelectionSet() const {
  return this->selectionSet;
}

// Document
Document::Document(
  cgqlContainer<Definition> definitions
): definitions(std::move(definitions)) {}

const cgqlContainer<Definition>& Document::getDefinitions() const {
  return this->definitions;
}

template<typename Ostream>
Ostream& operator<<(Ostream& out, OperationType type) {
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

static inline bool isList(const Data& type) {
  return type.index() == 3;
}

static inline bool isObject(const Data& type) {
  return type.index() == 2;
}

static inline bool isNull(const Data& type) {
  return type.index() == 4;
}

static void printSelectionSet(const SelectionSet& selectionSet, int level) {
  for(const auto& s : selectionSet) {
    std::string v;
    for(int i = 0; i < level; i++) v += "  ";
    if(s->getSelectionType() == SelectionType::FIELD) {
      cgqlSPtr<Field> field =
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
      OperationDefinition opDef =
        fromVariant<OperationDefinition>(def);
      logger::info(opDef.getOperationType());
      printSelectionSet(opDef.getSelectionSet(), 0);
    }
  }
}

static void printScalarValue(
  const std::string& key,
  const Data& value,
  const std::string& indentation
) {
  if(value.index() == 0) {
    std::string v = indentation;
    v += key;
    v += " ";
    v += std::to_string(fromVariant<Int>(value));
    logger::info(v);
  } else if(value.index() == 1) {
    std::string v = indentation;
    v += key;
    v += " ";
    v += fromVariant<String>(value);
    logger::info(v);
  }
}

static void printList(
  const std::string& key,
  const cgqlSPtr<List>& value,
  const std::string& indentation
) {
  logger::info(indentation + key);
  for(const auto& each : value->elements) {
    printScalarValue("", each, indentation);
  }
}

void printResultMap(const Object& obj, uint8_t level) {
  std::string indentation;
  for(int i = 0; i < level; i++) indentation += "  ";
  for(const auto& [key, value] : obj.fields) {
    if(isList(value)) {
      printList(key, fromVariant<cgqlSPtr<List>>(value), indentation);
    } else if(isObject(value)) {
      std::string v = indentation;
      v += key;
      logger::info(v);
      printResultMap(*fromVariant<std::shared_ptr<Object>>(value), level + 1);
    } else if(isNull(value)) {
      std::string v = indentation;
      v += key;
      v += " NULL";
      logger::info(v);
    } else {
      printScalarValue(key, value, indentation);
    }
  }
}

} // cgql
