#ifndef PARSING_PARSER__H
#define PARSING_PARSER__H

#include <istream>

#include "ast/ast_fwd.h"
#include "parsing/lexer.h"

namespace parsing {
class Parser {
 public:
  Parser(std::istream &input_stream) : _in_stream{input_stream} {}

 public:
  ast::CompilationUnitDecl *parse();

 private:
  std::istream &_in_stream;
  std::unique_ptr<Lexer> _lex;
};
}  // namespace parsing

#endif  // PARSING_PARSER__H
