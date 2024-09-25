#ifndef AST_TYPE__H
#define AST_TYPE__H

#include <cassert>
#include <cstdint>
#include <limits>

#include "ast/ast_fwd.h"
#include "reflect/model.h"

namespace ast {
struct Type {
  enum class Kind {
    kUnitType = 1001,
    kIntegralType,
    kClassType,
    kArrayType,
  };

  const Kind kind;

  Type(Kind kind) : kind{kind} {}

  META_INFO_NF(Type, 0, void);
};

struct UnitType : Type {
  UnitType() : Type{Kind::kUnitType} {}

  META_INFO_NF(UnitType, Kind::kUnitType, Type);
};

struct IntegralType : Type {
 private:
  const uint32_t _sign_width;

 public:
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
  ClassDecl *const cls;

  ClassType(ClassDecl *cls) : Type{Kind::kClassType}, cls{cls} {}

  META_INFO(ClassType, Kind::kClassType, Type, REF_FIELD(cls));
};

struct ArrayType : Type {
  Type *const element_type;
  const std::size_t size;

  ArrayType(Type *et, std::size_t size) : Type{Kind::kArrayType}, element_type{et}, size{size} {}

  META_INFO(ArrayType, Kind::kArrayType, Type, element_type, size);
};
}  // namespace ast

#endif  // AST_TYPE__H
