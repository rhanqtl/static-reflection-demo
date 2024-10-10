#ifndef AST_FWD__H
#define AST_FWD__H

#include <tuple>

namespace ast {
struct Type;
struct Decl;
struct Expr;
struct Stmt;

#define TYPE(x) struct x##Type;
#define DECL(x) struct x##Decl;
#define EXPR(x) struct x##Expr;
#define STMT(x) struct x##Stmt;

#include "ast_nodes.inc"

#undef TYPE
#undef DECL
#undef EXPR
#undef STMT

#define TYPE(x) x##Type,
using Types = std::tuple<
#include "ast_nodes.inc"
    void>;
#undef TYPE

#define DECL(x) x##Decl,
using Decls = std::tuple<
#include "ast_nodes.inc"
    void>;
#undef DECL

#define EXPR(x) x##Expr,
using Exprs = std::tuple<
#include "ast_nodes.inc"
    void>;
#undef EXPR

#define STMT(x) x##Stmt,
using Stmts = std::tuple<
#include "ast_nodes.inc"
    void>;
#undef STMT

#define TYPE(x) x##Type,
#define DECL(x) x##Decl,
#define EXPR(x) x##Expr,
#define STMT(x) x##Stmt,
using Nodes = std::tuple<
#include "ast_nodes.inc"
    void>;
#undef TYPE
#undef DECL
#undef EXPR
#undef STMT
}  // namespace ast

#endif  // AST_FWD__H
