#ifndef AST_FWD__H
#define AST_FWD__H

#include <tuple>

namespace ast {
struct Type;
struct UnitType;
struct IntegralType;
struct ClassType;
struct ArrayType;

struct Decl;
struct CompilationUnitDecl;
struct VarDecl;
struct FuncDecl;
struct ClassDecl;

struct Expr;
struct NumberLiteralExpr;
struct DeclRefExpr;
struct MemberExpr;
struct CallExpr;
struct UnaryExpr;
struct BinaryExpr;
struct IfExpr;
struct ForeachExpr;
struct BlockExpr;

struct Stmt;
struct ExprStmt;
struct DeclStmt;

using Types = std::tuple<UnitType, IntegralType, ClassType, ArrayType>;
using Decls = std::tuple<CompilationUnitDecl, VarDecl, FuncDecl, ClassDecl>;
using Exprs = std::tuple<NumberLiteralExpr, DeclRefExpr, MemberExpr, CallExpr, UnaryExpr,
                         BinaryExpr, IfExpr, ForeachExpr, BlockExpr>;
using Stmts = std::tuple<ExprStmt, DeclStmt>;
using Nodes = std::tuple<UnitType, IntegralType, ClassType, ArrayType, CompilationUnitDecl, VarDecl,
                         FuncDecl, ClassDecl, NumberLiteralExpr, DeclRefExpr, MemberExpr, CallExpr,
                         UnaryExpr, BinaryExpr, IfExpr, ForeachExpr, BlockExpr, ExprStmt, DeclStmt>;
}  // namespace ast

#endif  // AST_FWD__H
