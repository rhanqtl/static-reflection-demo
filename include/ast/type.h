#ifndef AST_TYPE__H
#define AST_TYPE__H

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "ast/ast_fwd.h"
#include "reflect/model.h"

namespace ast {
struct Type {
  enum class Kind {
    kUnitType = 1001,
    kIntegralType,
    kStringType,
    kClassType,
    kListType,
  };

  const Kind kind;

  Type(Kind kind) : kind{kind} {}

  template <typename Visitor>
  void accept(Visitor &v);

  META_INFO_NF(Type, 0, void);
};

struct UnitType : Type {
  UnitType() : Type{Kind::kUnitType} {}

  META_INFO_NF(UnitType, Kind::kUnitType, Type);
};

struct IntegralType : Type {
  uint32_t _sign_width;

 public:
  IntegralType() : IntegralType{false, 0} {}
  IntegralType(bool s, uint32_t w)
      : Type{Kind::kIntegralType}, _sign_width{combine_sign_width(s, w)} {}

  bool is_signed() const {
    return decompose_sign_width(_sign_width).first;
  }
  uint32_t width() const {
    return decompose_sign_width(_sign_width).second;
  }

 private:
  static uint32_t combine_sign_width(bool s, uint32_t w) {
    assert(w <= std::numeric_limits<int32_t>::max());
    if (s) return (1 << 31) | w;
    return w;
  }
  static std::pair<bool, uint32_t> decompose_sign_width(uint32_t sw) {
    bool s = (1 << 31) & sw;
    uint32_t w = ~(1 << 31) & sw;
    return {s, w};
  }

 public:
  META_INFO(IntegralType, Kind::kIntegralType, Type, _sign_width);
};

struct ClassType : Type {
  ClassDecl *cls;
  std::string name;

  ClassType() : ClassType{nullptr} {}
  ClassType(ClassDecl *cls) : Type{Kind::kClassType}, cls{cls} {}
  ClassType(std::string_view name) : Type{Kind::kClassType}, name{name}, cls{nullptr} {}

  META_INFO(ClassType, Kind::kClassType, Type, REF_FIELD(cls));
};

struct ListType : Type {
  Type *element_type;

  ListType() : ListType{nullptr} {}
  ListType(Type *et) : Type{Kind::kListType}, element_type{et} {}

  META_INFO(ListType, Kind::kListType, Type, element_type);
};

struct StringType : Type {
  StringType() : Type{Kind::kStringType} {}

  META_INFO_NF(StringType, Kind::kStringType, Type);
};
}  // namespace ast

namespace ast {
template <typename Visitor>
void Type::accept(Visitor &v) {
  switch (kind) {
#define TYPE(t)                             \
  case Kind::k##t##Type:                    \
    v.visit(*static_cast<t##Type *>(this)); \
    break;
#include "./ast_nodes.inc"
#undef TYPE
    default:
      std::cerr << this << ' ' << (void *)kind << '\n';
      assert(false);
  }
}
}  // namespace ast

#endif  // AST_TYPE__H
