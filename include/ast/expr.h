#ifndef AST_EXPR__H
#define AST_EXPR__H

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

#include "ast/ast_fwd.h"
#include "ast/decl.h"
#include "reflect/model.h"

namespace ast {
struct Expr {
  enum class Kind {
    kIntegerLiteralExpr = 3001,  // Number literals
    kStringLiteralExpr,
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

  virtual ~Expr() = default;

  Type *type() const {
    return nullptr;
  }

  template <typename Visitor>
  void accept(Visitor &v);

  META_INFO_NF(Expr, 0, void);
};

struct IntegerLiteralExpr : Expr {
  std::uint64_t value;

  IntegerLiteralExpr() : IntegerLiteralExpr{0} {}
  IntegerLiteralExpr(std::uint64_t value) : Expr{Kind::kIntegerLiteralExpr}, value{value} {}

  META_INFO(IntegerLiteralExpr, Kind::kIntegerLiteralExpr, Expr, value);
};

struct StringLiteralExpr : Expr {
  std::string value;

  StringLiteralExpr() : StringLiteralExpr{""} {}
  StringLiteralExpr(std::string_view value) : Expr{Kind::kStringLiteralExpr}, value{value} {}

  META_INFO(StringLiteralExpr, Kind::kStringLiteralExpr, Expr, value);
};

struct DeclRefExpr : Expr {
  Decl *decl;
  std::string name;

  DeclRefExpr() : DeclRefExpr{nullptr} {}
  DeclRefExpr(Decl *decl);
  DeclRefExpr(std::string_view name) : Expr{Kind::kDeclRefExpr}, name{name} {}

  META_INFO(DeclRefExpr, Kind::kDeclRefExpr, Expr, REF_FIELD(decl), name);
};

struct MemberExpr : Expr {
  Expr *prefix;
  Decl *target;
  std::string name;

  MemberExpr() : MemberExpr{nullptr, nullptr} {}
  MemberExpr(Expr *prefix, Decl *target);
  MemberExpr(Expr *prefix, std::string_view name)
      : Expr{Kind::kMemberExpr}, prefix{prefix}, name{name} {}

  META_INFO(MemberExpr, Kind::kMemberExpr, Expr, prefix, REF_FIELD(target), name);
};

struct CallExpr : Expr {
  Expr *callee;
  std::vector<Expr *> args;

  CallExpr() : CallExpr{nullptr, {}} {}
  CallExpr(Expr *callee, const std::vector<Expr *> &args)
      : Expr{Kind::kCallExpr}, callee{callee}, args{args} {}

  void add_argument(Expr *arg) {
    args.push_back(arg);
  }

  META_INFO(CallExpr, Kind::kCallExpr, Expr, callee, args);
};

struct UnaryExpr : Expr {
  enum OpCode {
    kUndef,
    kLogicalNot,  // !
    kNeg,         // -
  };

  OpCode op;
  Expr *arg;

  UnaryExpr() : UnaryExpr{OpCode::kUndef, nullptr} {}
  UnaryExpr(OpCode op, Expr *arg) : Expr{Kind::kUnaryExpr}, op{op}, arg{arg} {}

  META_INFO(UnaryExpr, Kind::kUnaryExpr, Expr, op, arg);
};

struct BinaryExpr : Expr {
  enum OpCode {
    kUndef,
    kAdd,       // +
    kSub,       // -
    kNotEqual,  // !=
  };

  OpCode op;
  Expr *lhs;
  Expr *rhs;

  BinaryExpr() : BinaryExpr{OpCode::kUndef, nullptr, nullptr} {}
  BinaryExpr(OpCode op, Expr *lhs, Expr *rhs)
      : Expr{Kind::kBinaryExpr}, op{op}, lhs{lhs}, rhs{rhs} {}

  META_INFO(BinaryExpr, Kind::kBinaryExpr, Expr, op, lhs, rhs);
};

struct IfExpr : Expr {
  Expr *cond;
  BlockExpr *then_br;
  BlockExpr *else_br;

  IfExpr() : IfExpr(nullptr, nullptr, nullptr) {}
  IfExpr(Expr *cond, BlockExpr *then_br, BlockExpr *else_br)
      : Expr{Kind::kIfExpr}, cond{cond}, then_br{then_br}, else_br{else_br} {}

  META_INFO(IfExpr, Kind::kIfExpr, Expr, cond, then_br, else_br);
};

struct ForeachExpr : Expr {
  VarDecl *iterator;
  Expr *seq;
  BlockExpr *body;

  ForeachExpr() : ForeachExpr{nullptr, nullptr, nullptr} {}
  ForeachExpr(VarDecl *iterator, Expr *seq, BlockExpr *body)
      : Expr{Kind::kForeachExpr}, iterator{iterator}, seq{seq}, body{body} {}

  META_INFO(ForeachExpr, Kind::kForeachExpr, Expr, /* It's owned, not a typo */ iterator, seq,
            body);
};

struct BlockExpr : Expr {
  std::vector<Stmt *> stmts;
  Expr *last_expr;

  BlockExpr() : BlockExpr({}, nullptr) {}
  BlockExpr(const std::vector<Stmt *> &stmts) : BlockExpr{stmts, nullptr} {}
  BlockExpr(Expr *last_expr) : BlockExpr({}, last_expr) {}
  BlockExpr(const std::vector<Stmt *> &stmts, Expr *last_expr)
      : Expr{Kind::kBlockExpr}, stmts{stmts}, last_expr{last_expr} {}

  void add_stmt(Stmt *stmt) {
    stmts.push_back(stmt);
  }
  void set_expr(Expr *expr) {
    last_expr = expr;
  }

  META_INFO(BlockExpr, Kind::kBlockExpr, Expr, stmts, last_expr);
};
}  // namespace ast

namespace ast {
template <typename Visitor>
void Expr::accept(Visitor &v) {
  switch (kind) {
#define EXPR(t)                             \
  case Kind::k##t##Expr:                    \
    v.visit(*static_cast<t##Expr *>(this)); \
    break;
#include "./ast_nodes.inc"
#undef EXPR
  }
}
}  // namespace ast

#endif  // AST_EXPR__H
