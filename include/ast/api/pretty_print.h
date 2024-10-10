#ifndef AST_API_PRETTY_PRINT__H
#define AST_API_PRETTY_PRINT__H

#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "ast/api/visitor.h"
#include "utility/save_restore.h"

namespace ast {
class PrettyPrintVisitor : private Visitor<> {
 public:
  PrettyPrintVisitor(std::ostream& out_stream) : _out_stream{out_stream} {}

 public:
#define TYPE(x) void visit(x##Type&) override;
#define DECL(x) void visit(x##Decl&) override;
#define EXPR(x) void visit(x##Expr&) override;
#define STMT(x) void visit(x##Stmt&) override;
#include "../ast_nodes.inc"
#undef TYPE
#undef DECL
#undef EXPR
#undef STMT

 private:
  void print_indent() {
    _out_stream << _indent;
  }

  template <typename T>
  void print_vector(const std::vector<T>& xs, const std::string& delim) {
    auto first = true;
    for (auto& x : xs) {
      if (first)
        first = false;
      else
        _out_stream << delim;

      traverse_node(x);
    }
  }

  void print_body(BlockExpr* block) {
    SAVE_RESTORE(_indent_opening_brace, true);
    traverse_node(block);
  }

 private:
  std::ostream& _out_stream;

  std::string _indent;
  bool _indent_opening_brace{true};
};

template <typename T>
std::string to_string(const T& object) {
  std::ostringstream ss;
  auto pp = PrettyPrintVisitor{ss};
  const_cast<T&>(object).accept(pp);
  return ss.str();
}
}  // namespace ast

#define NEXT_LEVEL() SAVE_RESTORE(_indent, _indent + "  ")

namespace ast {
void PrettyPrintVisitor::visit(UnitType& node) {
  _out_stream << "()";
}

void PrettyPrintVisitor::visit(IntegralType& node) {
  assert(node.is_signed() && node.width() == 32);
  _out_stream << "i32";
}

void PrettyPrintVisitor::visit(StringType& node) {
  _out_stream << "string";
}

void PrettyPrintVisitor::visit(ClassType& node) {
  if (node.cls)
    _out_stream << node.cls->name;
  else if (!node.name.empty())
    _out_stream << node.name;
  else
    assert(0);
}

void PrettyPrintVisitor::visit(ListType& node) {
  _out_stream << "[]";
  traverse_node(node.element_type);
}

void PrettyPrintVisitor::visit(CompilationUnitDecl& node) {
  Visitor<>::visit(node);
}

void PrettyPrintVisitor::visit(VarDecl& node) {
  print_indent();

  _out_stream << "var" << ' ';
  _out_stream << node.name;

  _out_stream << ':' << ' ';
  traverse_node(node.type);

  if (node.init_val) {
    _out_stream << ' ' << '=' << ' ';
    traverse_node(node.init_val);
  }

  _out_stream << ';';
}

void PrettyPrintVisitor::visit(FuncDecl& node) {
  print_indent();

  _out_stream << "func" << ' ';
  _out_stream << node.name;

  _out_stream << '(';
  bool first = true;
  for (const auto& param_spec : node.params) {
    if (first)
      first = false;
    else
      _out_stream << ',' << ' ';

    auto&& [name, type] = param_spec;
    _out_stream << name;
    _out_stream << ':' << ' ';
    traverse_node(type);
  }
  _out_stream << ')';

  _out_stream << ' ' << "->" << ' ';
  traverse_node(node.return_type);

  _out_stream << ' ';
  traverse_node(node.body);
}

void PrettyPrintVisitor::visit(ClassDecl& node) {
  _out_stream << "class" << ' ';
  _out_stream << node.name;

  _out_stream << '{';
  if (!node.vars.empty() || !node.funcs.empty()) {
    NEXT_LEVEL();

    if (!node.vars.empty()) {
      _out_stream << '\n';
      print_vector(node.vars, "\n");
    }

    if (!node.funcs.empty()) {
      _out_stream << '\n';
      print_vector(node.funcs, "\n");
    }
  }
  _out_stream << '}';
}

void PrettyPrintVisitor::visit(IntegerLiteralExpr& node) {
  _out_stream << node.value;
}

void PrettyPrintVisitor::visit(StringLiteralExpr& node) {
  _out_stream << node.value;
}

void PrettyPrintVisitor::visit(DeclRefExpr& node) {
  if (node.decl)
    _out_stream << node.decl->name;
  else if (!node.name.empty())
    _out_stream << node.name;
  else
    assert(0);
}

void PrettyPrintVisitor::visit(MemberExpr& node) {
  traverse_node(node.prefix);

  _out_stream << '.';

  if (node.target)
    _out_stream << node.target->name;
  else if (!node.name.empty())
    _out_stream << node.name;
  else
    assert(0);
}

void PrettyPrintVisitor::visit(CallExpr& node) {
  traverse_node(node.callee);

  _out_stream << '(';
  print_vector(node.args, ", ");
  _out_stream << ')';
}

void PrettyPrintVisitor::visit(UnaryExpr& node) {
  using Op = ast::UnaryExpr::OpCode;
  switch (node.op) {
    case Op::kLogicalNot:
      _out_stream << '!';
      break;
    case Op::kNeg:
      _out_stream << '-';
      break;
    default:
      assert(0);
  }
  traverse_node(node.arg);
}

void PrettyPrintVisitor::visit(BinaryExpr& node) {
  traverse_node(node.lhs);

  _out_stream << ' ';

  using Op = ast::BinaryExpr::OpCode;
  switch (node.op) {
    case Op::kAdd:
      _out_stream << '+';
      break;
    case Op::kSub:
      _out_stream << '-';
      break;
    case Op::kNotEqual:
      _out_stream << "!=";
      break;
    default:
      assert(0);
  }

  _out_stream << ' ';

  traverse_node(node.rhs);
}

void PrettyPrintVisitor::visit(IfExpr& node) {
  _out_stream << "if";

  _out_stream << ' ';
  traverse_node(node.cond);
  _out_stream << ' ';

  traverse_node(node.then_br);

  if (node.else_br) {
    _out_stream << ' ' << "else" << ' ';
    print_body(node.else_br);
  }
}

void PrettyPrintVisitor::visit(ForeachExpr& node) {
  _out_stream << "foreach" << ' ' << node.iterator->name << ' ' << "in" << ' ';
  traverse_node(node.seq);
  _out_stream << ' ';
  print_body(node.body);
}

void PrettyPrintVisitor::visit(BlockExpr& node) {
  if (_indent_opening_brace)
    print_indent();
  _out_stream << '{' << '\n';

  {
    NEXT_LEVEL();
    print_vector(node.stmts, "\n");
    if (node.last_expr) {
      print_indent();
      traverse_node(node.last_expr);
    }
    _out_stream << '\n';
  }

  print_indent();
  _out_stream << '}';
}

void PrettyPrintVisitor::visit(ExprStmt& node) {
  print_indent();
  traverse_node(node.expr);
}

void PrettyPrintVisitor::visit(DeclStmt& node) {
  traverse_node(node.decl);
}

void PrettyPrintVisitor::visit(ReturnStmt& node) {
  print_indent();
  traverse_node(node.expr);
}
}  // namespace ast

#endif  // AST_API_PRETTY_PRINT__H
