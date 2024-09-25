#ifndef AST_STMT__H
#define AST_STMT__H

#include "ast/ast_fwd.h"
#include "reflect/model.h"

namespace ast {
struct Stmt {
  enum class Kind {
    kExprStmt = 4001,
    kDeclStmt,
  };

  const Kind kind;

  Stmt(Kind kind) : kind{kind} {}

  META_INFO_NF(Stmt, 0, void);
};

struct ExprStmt : Stmt {
  Expr *const expr;

  ExprStmt(Expr *expr) : Stmt{Kind::kExprStmt}, expr{expr} {}

  META_INFO(ExprStmt, Kind::kExprStmt, Stmt, expr);
};

struct DeclStmt : Stmt {
  VarDecl *const decl;

  DeclStmt(VarDecl *decl) : Stmt{Kind::kDeclStmt}, decl{decl} {}

  META_INFO(DeclStmt, Kind::kDeclStmt, Stmt, decl);
};
}  // namespace ast

#endif  // AST_STMT__H
