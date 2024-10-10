#ifndef AST_STMT__H
#define AST_STMT__H

#include "ast/ast_fwd.h"
#include "reflect/model.h"

namespace ast {
template <typename R>
class Visitor;
}

namespace ast {
struct Stmt {
  enum class Kind {
    kExprStmt = 4001,
    kDeclStmt,
    kReturnStmt,
  };

  const Kind kind;

  Stmt(Kind kind) : kind{kind} {}

  template <typename Visitor>
  void accept(Visitor &v);

  META_INFO_NF(Stmt, 0, void);
};

struct ExprStmt : Stmt {
  Expr *expr;

  ExprStmt() : ExprStmt{nullptr} {}
  ExprStmt(Expr *expr) : Stmt{Kind::kExprStmt}, expr{expr} {}

  META_INFO(ExprStmt, Kind::kExprStmt, Stmt, expr);
};

struct DeclStmt : Stmt {
  VarDecl *decl;

  DeclStmt() : DeclStmt{nullptr} {}
  DeclStmt(VarDecl *decl) : Stmt{Kind::kDeclStmt}, decl{decl} {}

  META_INFO(DeclStmt, Kind::kDeclStmt, Stmt, decl);
};

struct ReturnStmt : Stmt {
  Expr *expr;

  ReturnStmt() : ReturnStmt{nullptr} {}
  ReturnStmt(Expr *expr) : Stmt{Kind::kReturnStmt}, expr{expr} {}

  META_INFO(ReturnStmt, Kind::kReturnStmt, Stmt, expr);
};
}  // namespace ast

namespace ast {
template <typename Visitor>
void Stmt::accept(Visitor &v) {
  switch (kind) {
#define STMT(t)                             \
  case Kind::k##t##Stmt:                    \
    v.visit(*static_cast<t##Stmt *>(this)); \
    break;
#include "./ast_nodes.inc"
#undef STMT
  }
}
}  // namespace ast

#endif  // AST_STMT__H
