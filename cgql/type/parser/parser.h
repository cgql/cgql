#include "../../cgqlPch.h"

#include "../Document.h"
#include "tokenizer.h"

using std::string;

class Parser {
public:
  Parser(const char* document);
  ~Parser();
  Document parseDocument();
private:
  Token move(TokenType type);
  bool moveUntil(TokenType type);
  bool checkType(TokenType type);

  string document;
  Tokenizer tokenizer;

  OperationDefinition parseOperationDefinition();
  SelectionSet parseSelectionSet();
  Selection parseSelection();
  Field* parseField();

  string parseName();
};

Document parse(const char* source);
