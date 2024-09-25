#ifndef AST_DECL__H
#define AST_DECL__H

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "ast/ast_fwd.h"
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
  const std::string name;
  std::unordered_set<void *> users;

  void add_user(void *u) {
    users.emplace(u);
  }

  META_INFO(Decl, 0, void, name, TRANSIENT_FIELD(users));
};
inline bool Decl::update_users = true;

struct CompilationUnitDecl : Decl {
  std::vector<std::unique_ptr<Decl>> decls;

  using Decl::Decl;

  META_INFO_NF(CompilationUnitDecl, Kind::kCompilationUnitDecl, Decl);
};

struct VarDecl : Decl {
  Type *type;

  VarDecl(std::string_view name, Type *type) : Decl{Kind::kVarDecl, name}, type{type} {}

  META_INFO(VarDecl, Kind::kVarDecl, Decl, type);
};

struct FuncDecl : Decl {
  const std::vector<Type *> param_types;
  Type *const return_type;

  FuncDecl(std::string_view name, Type *return_type, const std::vector<Type *> &param_types)
      : Decl{Kind::kFuncDecl, name}, param_types{param_types}, return_type{return_type} {}

  META_INFO(FuncDecl, Kind::kFuncDecl, Decl, param_types, return_type);
};

struct ClassDecl : Decl {
  std::vector<VarDecl *> vars;
  std::vector<FuncDecl *> funcs;

  ClassDecl(std::string_view name) : Decl{Kind::kClassDecl, name} {}

  META_INFO(ClassDecl, Kind::kClassDecl, Decl, vars, funcs);
};
}  // namespace ast

#endif  // AST_DECL__H
