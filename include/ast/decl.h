#ifndef AST_DECL__H
#define AST_DECL__H

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "ast/ast_fwd.h"
#include "ast/stmt.h"
#include "reflect/model.h"

namespace ast {
struct Decl {
  enum class Kind {
    kCompilationUnitDecl = 2001,
    kVarDecl,
    kFuncDecl,
    kClassDecl,
  };

 protected:
  Decl(Kind kind, std::string_view name) : kind{kind}, name{name} {}

 public:
  static bool update_users;

  const Kind kind;
  std::string name;
  std::unordered_set<void *> users;

  void add_user(void *u) {
    users.emplace(u);
  }

  template <typename Visitor>
  void accept(Visitor &v);

  META_INFO(Decl, 0, void, name, TRANSIENT_FIELD(users));
};
inline bool Decl::update_users = true;

struct CompilationUnitDecl : Decl {
  std::vector<Decl *> decls;

  CompilationUnitDecl() : CompilationUnitDecl{""} {}
  CompilationUnitDecl(std::string_view name) : Decl{Kind::kCompilationUnitDecl, name} {}

  META_INFO(CompilationUnitDecl, Kind::kCompilationUnitDecl, Decl, decls);
};

struct VarDecl : Decl {
  Type *type;
  Expr *init_val;

  VarDecl() : VarDecl{"", nullptr} {}
  VarDecl(std::string_view name, Type *type, Expr *init_val = nullptr)
      : Decl{Kind::kVarDecl, name}, type{type}, init_val{init_val} {}

  META_INFO(VarDecl, Kind::kVarDecl, Decl, type);
};

struct FuncDecl : Decl {
  struct ParamSpec {
    std::string_view name;
    Type *type;
  };

  std::vector<std::tuple<std::string, Type *>> params;
  Type *return_type;
  BlockExpr *body;

  FuncDecl() : FuncDecl{"", {}, nullptr, nullptr} {}
  FuncDecl(std::string_view name, const std::vector<ParamSpec> &param_specs, Type *return_type,
           BlockExpr *body)
      : Decl{Kind::kFuncDecl, name}, return_type{return_type}, body{body} {
    params.reserve(param_specs.size());
    for (const auto &spec : param_specs) {
      params.emplace_back(spec.name, spec.type);
    }
  }

  // void add_stmt(Stmt *stmt);

  META_INFO(FuncDecl, Kind::kFuncDecl, Decl, params, return_type, body);
};

struct ClassDecl : Decl {
  std::vector<VarDecl *> vars;
  std::vector<FuncDecl *> funcs;

  ClassDecl() : ClassDecl{""} {}
  ClassDecl(std::string_view name) : Decl{Kind::kClassDecl, name} {}

  META_INFO(ClassDecl, Kind::kClassDecl, Decl, vars, funcs);
};
}  // namespace ast

namespace ast {
template <typename Visitor>
void Decl::accept(Visitor &v) {
  switch (kind) {
#define DECL(t)                             \
  case Kind::k##t##Decl:                    \
    v.visit(*static_cast<t##Decl *>(this)); \
    break;
#include "./ast_nodes.inc"
#undef DECL
  }
}
}  // namespace ast

#endif  // AST_DECL__H
