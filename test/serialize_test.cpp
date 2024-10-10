#include "serde/serialize.h"

#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "ast/api/pretty_print.h"
#include "ast/decl.h"
#include "ast/expr.h"
#include "ast/type.h"
#include "pool.h"
#include "serde/deserialize.h"

TEST(Serialization, It_Compiles) {
  serde::ASTSaver saver{"."};
  saver.save();
}

TEST(Deserialization, It_Compiles) {
  serde::ASTLoader loader{"."};
  loader.load();
}

// func add(a: i32, b: i32) -> i32 {
//   a + b
// }
TEST(Serialization, Basic) {
  auto param_a_type = ast::Pool<ast::IntegralType>::instance().create(true, 32);
  auto param_a_spec = ast::FuncDecl::ParamSpec{"a", param_a_type};

  auto param_b_type = ast::Pool<ast::IntegralType>::instance().create(true, 32);
  auto param_b_spec = ast::FuncDecl::ParamSpec{"b", param_b_type};

  auto param_specs = std::vector{param_a_spec, param_b_spec};

  auto ret_type = ast::Pool<ast::IntegralType>::instance().create(true, 32);

  auto body =
      ast::Pool<ast::BlockExpr>::instance().create(ast::Pool<ast::BinaryExpr>::instance().create(
          ast::BinaryExpr::OpCode::kAdd, ast::Pool<ast::DeclRefExpr>::instance().create("a"),
          ast::Pool<ast::DeclRefExpr>::instance().create("b")));

  auto fn_add = ast::Pool<ast::FuncDecl>::instance().create("add", param_specs, ret_type, body);

  auto *cu = ast::Pool<ast::CompilationUnitDecl>::instance().create("_unit_");
  cu->decls.push_back(fn_add);

  auto dir = std::filesystem::path{testing::TempDir()};

  serde::ASTSaver saver{dir};
  saver.save();
}

TEST(Deserialization, Basic) {
  auto dir = std::filesystem::path{testing::TempDir()};

  serde::ASTLoader loader{dir};
  loader.load();

  ASSERT_EQ(ast::Pool<ast::CompilationUnitDecl>::instance().num_nodes(), 1);

  auto &cu = ast::Pool<ast::CompilationUnitDecl>::instance().at(0);
  EXPECT_EQ(ast::to_string(cu),
            "func add(a: i32, b: i32) -> i32 {\n"
            "  a + b\n"
            "}");
}
