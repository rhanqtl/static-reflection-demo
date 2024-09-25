#ifndef SERDE_DESERIALIZE__H
#define SERDE_DESERIALIZE__H

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "ast/ast_fwd.h"
#include "ast/decl.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/type.h"
#include "pool.h"
#include "reflect/access.h"
#include "serde/decoder.h"

inline std::unordered_map<std::uintptr_t, std::vector<std::pair<void *, void **>>>
    rfe_old_addr_to_rfr{};
struct IndexEntry {
  std::uintptr_t old_addr;
  void *new_addr;
};
inline std::unordered_map</* Class ID */ int, std::vector<IndexEntry>> addr_mapping{};

namespace serde {
class ASTLoader {
 public:
  ASTLoader(const std::filesystem::path &dir) : _dir{dir} {}

  void load() {
    // 1. Load AST nodes.
    ast::Decl::update_users = false;
    load_pools(std::make_index_sequence<std::tuple_size_v<ast::Nodes>>());

    // 2. Load old addr info.
    auto index_stream = std::ifstream{_dir / "index.db"};
    while (index_stream) {
      int cls_id;
      index_stream >> cls_id;
      auto &table = addr_mapping[cls_id];
      std::size_t n_entries;
      index_stream >> n_entries;
      while (index_stream) {
        int cls_id;
        index_stream >> cls_id;
        auto &table = addr_mapping[cls_id];
        std::size_t n_entries;
        index_stream >> n_entries;
        if (table.empty())
          table.resize(n_entries);
        else
          assert(table.size() == n_entries);
        for (std::size_t i = 0; i < n_entries; i++) {
          index_stream >> table[i].old_addr;
        }
      }
    }

    // 3. Patch and update users.
    for (const auto &[cls_id, table] : addr_mapping) {
      for (auto [old_addr, new_addr] : table) {
        auto it = rfe_old_addr_to_rfr.find(old_addr);
        if (it == rfe_old_addr_to_rfr.end()) continue;
        for (auto [user, slot] : it->second) {
          *slot = new_addr;
          add_user(cls_id, new_addr, user);
        }
      }
    }
  }

 private:
  template <std::size_t... Is>
  void load_pools(std::index_sequence<Is...>) {
    (load_pool<std::tuple_element_t<Is, ast::Nodes>>(), ...);
  }

  template <typename T>
  void load_pool() {
    auto &pool = ast::Pool<T>::instance();
    auto p = _dir / T::kClassName;
    assert(std::filesystem::exists(p));
    std::ifstream in_s{p, std::ios::binary};
    std::size_t n_nodes;
    in_s >> n_nodes;
    pool.reserve(n_nodes);
    auto &table = addr_mapping[T::kClassID];
    table.resize(n_nodes);
    pool.for_each([&in_s, &table](std::size_t i, T &object) {
      table[i].new_addr = &object;
      load_node(in_s, object);
    });
  }

  template <typename T>
  static void load_node(std::istream &in_s, T &object) {
    detail::DataDecoder<T>{}(in_s, object);
  }

  void add_user(int class_id, void *new_addr, void *user) {
    switch (class_id) {
      case ast::ClassDecl::kClassID:
        [[fallthrough]];
      case ast::VarDecl::kClassID:
        [[fallthrough]];
      case ast::FuncDecl::kClassID:
        reinterpret_cast<ast::Decl *>(new_addr)->add_user(user);
      default:
        assert(false);
    }
  }

 private:
  std::filesystem::path _dir;
};
}  // namespace serde

#endif  // SERDE_DESERIALIZE__H
