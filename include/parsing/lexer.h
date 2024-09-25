#ifndef PARSING_LEXER__H
#define PARSING_LEXER__H

#include <istream>

namespace parsing {
class Lexer {
 public:
  Lexer(std::istream &input_stream) : _in_stream{input_stream} {}

 public:
  Token next_token();

 private:
  std::istream &_in_stream;
};
}  // namespace parsing

#endif  // PARSING_LEXER__H
