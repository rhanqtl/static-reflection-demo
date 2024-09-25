#ifndef AST_EXPR__H
#define AST_EXPR__H

#include <cassert>
#include <cstdint>
#include <vector>

#include "ast/ast_fwd.h"
#include "reflect/model.h"

namespace ast {
struct Expr {
  enum class Kind {
    kNumberLiteralExpr = 3001,  // Number literals
    kDeclRefExpr,
    kMemberExpr,  // a.b.c
    kCallExpr,
    kUnaryExpr,
    kBinaryExpr,
    kIfExpr,
    kForeachExpr,
    kBlockExpr,  // { ... }
  };

  Kind kind;

  Expr(Kind kind) : kind{kind} {}

  META_INFO_NF(Expr, 0, void);
};

struct NumberLiteralExpr : Expr {
  const std::uint64_t value;

  NumberLiteralExpr(std::uint64_t value) : Expr{Kind::kNumberLiteralExpr}, value{value} {}

  META_INFO(NumberLiteralExpr, Kind::kNumberLiteralExpr, Expr, value);
};

struct DeclRefExpr : Expr {
  Decl *decl;

  DeclRefExpr(Decl *decl);

  META_INFO(DeclRefExpr, Kind::kDeclRefExpr, Expr, REF_FIELD(decl));
};

struct MemberExpr : Expr {
  Expr *prefix;
  Decl *target;

  MemberExpr(Expr *prefix, Decl *target);

  META_INFO(MemberExpr, Kind::kMemberExpr, Expr, prefix, REF_FIELD(target));
};

struct CallExpr : Expr {
  Expr *callee;
  std::vector<Expr *> args;

  CallExpr(Expr *callee, const std::vector<Expr *> &args)
      : Expr{Kind::kCallExpr}, callee{callee}, args{args} {}

  META_INFO(CallExpr, Kind::kCallExpr, Expr, callee, args);
};

struct UnaryExpr : Expr {
  Expr *arg;

  UnaryExpr(Expr *arg) : Expr{Kind::kUnaryExpr}, arg{arg} {}

  META_INFO(UnaryExpr, Kind::kUnaryExpr, Expr, arg);
};

struct BinaryExpr : Expr {
  Expr *lhs;
  Expr *rhs;

  BinaryExpr(Expr *lhs, Expr *rhs) : Expr{Kind::kBinaryExpr}, lhs{lhs}, rhs{rhs} {}

  META_INFO(BinaryExpr, Kind::kBinaryExpr, Expr, lhs, rhs);
};

struct IfExpr : Expr {
  Expr *cond;
  Expr *then_br;
  Expr *else_br;

  IfExpr(Expr *cond, Expr *then_br, Expr *else_br)
      : Expr{Kind::kIfExpr}, cond{cond}, then_br{then_br}, else_br{else_br} {}

  META_INFO(IfExpr, Kind::kIfExpr, Expr, cond, then_br, else_br);
};

struct ForeachExpr : Expr {
  VarDecl *iterator;
  Expr *seq_ref;
  Expr *body;

  META_INFO(ForeachExpr, Kind::kForeachExpr, Expr, /* It's owned, not a typo */ iterator, seq_ref,
            body);
};

struct BlockExpr : Expr {
  std::vector<Expr *> exprs;

  BlockExpr(const std::vector<Expr *> &exprs) : Expr{Kind::kBlockExpr}, exprs{exprs} {}

  META_INFO(BlockExpr, Kind::kBlockExpr, Expr, exprs);
};
}  // namespace ast

#endif  // AST_EXPR__H
