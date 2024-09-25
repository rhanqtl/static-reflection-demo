#include "ast/expr.h"

#include "ast/decl.h"

namespace ast {
DeclRefExpr::DeclRefExpr(Decl *decl) : Expr{Kind::kDeclRefExpr}, decl{decl} {
  if (Decl::update_users) decl->add_user(this);
}

MemberExpr::MemberExpr(Expr *prefix, Decl *target)
    : Expr{Kind::kMemberExpr}, prefix{prefix}, target{target} {
  // assert(prefix && "MemberExpr::prefix shall be non-null");
  if (Decl::update_users) target->add_user(this);
}
}  // namespace ast
