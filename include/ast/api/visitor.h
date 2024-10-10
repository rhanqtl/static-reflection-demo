#ifndef AST_API_VISITOR__H
#define AST_API_VISITOR__H

#include <type_traits>
#include <vector>

#include "ast/decl.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/type.h"

namespace ast {
template <typename R = void>
class Visitor {
 public:
  virtual R visit(Type&);
  virtual R visit(Decl&);
  virtual R visit(Expr&);
  virtual R visit(Stmt&);

#define TYPE(x) virtual R visit(x##Type&);
#define DECL(x) virtual R visit(x##Decl&);
#define EXPR(x) virtual R visit(x##Expr&);
#define STMT(x) virtual R visit(x##Stmt&);
#include "../ast_nodes.inc"
#undef TYPE
#undef DECL
#undef EXPR
#undef STMT

#if 0
  virtual R visit(ast::Type&);
  virtual R visit(ast::UnitType&);
  virtual R visit(ast::IntegralType&);
  virtual R visit(ast::ClassType&);
  virtual R visit(ast::ArrayType&);

  virtual R visit(ast::Decl&);
  virtual R visit(ast::CompilationUnitDecl&);
  virtual R visit(ast::VarDecl&);
  virtual R visit(ast::FuncDecl&);
  virtual R visit(ast::ClassDecl&);

  virtual R visit(ast::Expr&);
  virtual R visit(ast::NumberLiteralExpr&);
  virtual R visit(ast::DeclRefExpr&);
  virtual R visit(ast::MemberExpr&);
  virtual R visit(ast::CallExpr&);
  virtual R visit(ast::UnaryExpr&);
  virtual R visit(ast::BinaryExpr&);
  virtual R visit(ast::IfExpr&);
  virtual R visit(ast::ForeachExpr&);
  virtual R visit(ast::BlockExpr&);

  virtual R visit(ast::Stmt&);
  virtual R visit(ast::ExprStmt&);
  virtual R visit(ast::DeclStmt&);
  virtual R visit(ast::ReturnStmt&);
#endif

 protected:
  template <typename T>
  void traverse_vector(const std::vector<T>& xs) {
    for (auto& x : xs) {
      traverse_node(x);
    }
  }

  template <typename T>
  void traverse_node(T* x) {
    if (!x) return;
    x->accept(*this);
  }
};
}  // namespace ast

namespace ast {
template <typename R>
R Visitor<R>::visit(Type& node) {}

template <typename R>
R Visitor<R>::visit(UnitType& node) {}

template <typename R>
R Visitor<R>::visit(IntegralType&) {}

template <typename R>
R Visitor<R>::visit(StringType&) {}

template <typename R>
R Visitor<R>::visit(ClassType&) {}

template <typename R>
R Visitor<R>::visit(ListType& node) {
  traverse_node(node.element_type);
}

template <typename R>
R Visitor<R>::visit(Decl&) {}

template <typename R>
R Visitor<R>::visit(CompilationUnitDecl& node) {
  traverse_vector(node.decls);
}

template <typename R>
R Visitor<R>::visit(VarDecl& node) {
  // node.type->accept(*this);
}

template <typename R>
R Visitor<R>::visit(FuncDecl& node) {
  traverse_node(node.body);
}

template <typename R>
R Visitor<R>::visit(ClassDecl& node) {
  traverse_vector(node.vars);
  traverse_vector(node.funcs);
}

template <typename R>
R Visitor<R>::visit(Expr&) {}

template <typename R>
R Visitor<R>::visit(IntegerLiteralExpr&) {}

template <typename R>
R Visitor<R>::visit(StringLiteralExpr&) {}

template <typename R>
R Visitor<R>::visit(DeclRefExpr&) {}

template <typename R>
R Visitor<R>::visit(MemberExpr& node) {
  traverse_node(node.prefix);
}

template <typename R>
R Visitor<R>::visit(CallExpr& node) {
  traverse_node(node.callee);
  traverse_vector(node.args);
}

template <typename R>
R Visitor<R>::visit(UnaryExpr& node) {
  traverse_node(node.arg);
}

template <typename R>
R Visitor<R>::visit(BinaryExpr& node) {
  traverse_node(node.lhs);
  traverse_node(node.rhs);
}

template <typename R>
R Visitor<R>::visit(IfExpr& node) {
  traverse_node(node.cond);
  traverse_node(node.then_br);
  traverse_node(node.else_br);
}

template <typename R>
R Visitor<R>::visit(ForeachExpr& node) {
  traverse_node(node.iterator);
  traverse_node(node.seq);
  traverse_node(node.body);
}

template <typename R>
R Visitor<R>::visit(BlockExpr& node) {
  traverse_vector(node.stmts);
  traverse_node(node.last_expr);
}

template <typename R>
R Visitor<R>::visit(Stmt&) {}

template <typename R>
R Visitor<R>::visit(ExprStmt& node) {
  traverse_node(node.expr);
}

template <typename R>
R Visitor<R>::visit(DeclStmt& node) {
  traverse_node(node.decl);
}

template <typename R>
R Visitor<R>::visit(ReturnStmt& node) {
  traverse_node(node.expr);
}
}  // namespace ast

#endif  // AST_API_ R visitOR__H
