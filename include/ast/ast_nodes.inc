#ifndef TYPE
#  define TYPE(x)
#endif

TYPE(Unit)
TYPE(Integral)
TYPE(Class)
TYPE(List)
TYPE(String)

#undef TYPE

#ifndef DECL
#  define DECL(x)
#endif

DECL(CompilationUnit)
DECL(Var)
DECL(Func)
DECL(Class)

#undef DECL

#ifndef EXPR
#  define EXPR(x)
#endif

EXPR(IntegerLiteral)
EXPR(StringLiteral)
EXPR(DeclRef)
EXPR(Member)
EXPR(Call)
EXPR(Unary)
EXPR(Binary)
EXPR(If)
EXPR(Foreach)
EXPR(Block)

#undef EXPR

#ifndef STMT
#  define STMT(x)
#endif

STMT(Expr)
STMT(Decl)
STMT(Return)

#undef STMT
