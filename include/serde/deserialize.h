#ifndef SERDE_DESERIALIZE__H
#define SERDE_DESERIALIZE__H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
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
#include "serde/io.h"
#include "utility/logging.h"

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
    INFO("Loading pools");
    ast::Decl::update_users = false;
    load_pools(std::make_index_sequence<std::tuple_size_v<ast::Nodes> - 1>());

    // 2. Load old addr info.
    INFO("Loading address mapping");
    auto index_stream = std::ifstream{_dir / "index.db", std::ios::binary};
    index_stream.seekg(0, std::ios::end);
    const auto file_end = index_stream.tellg();
    index_stream.seekg(0, std::ios::beg);
    while (index_stream.tellg() != file_end) {
      const int cls_id = io::detail::read<int32_t>(index_stream);
      DEBUG("offset={}", (std::size_t)index_stream.tellg());
      const std::size_t n_entries = io::read_size(index_stream);
      DEBUG("{} has {} nodes", cls_id, n_entries);
      auto &table = addr_mapping[cls_id];
      table.resize(n_entries);
      for (std::size_t i = 0; i < n_entries; i++) {
        table[i].old_addr = io::read_u64(index_stream);
      }
    }

#if 0
    for (const auto &[class_id, table] : addr_mapping) {
      DEBUG("Class ID: {}", class_id);
      for (const auto &entry : table) {
        DEBUG("  old_addr={}", (void *)entry.old_addr);
      }
    }
#endif
#if 0
    for (const auto &[old_addr, ps] : rfe_old_addr_to_rfr) {
      std::cerr << "old_addr=" << (void *)old_addr << '\n';
      for (const auto &[user, slot] : ps) {
        std::cerr << "  user=" << user << ' ' << "slot=" << slot << '\n';
      }
    }
#endif
    // 3. Patch and update users.
    INFO("Start back-patching");
    for (const auto &[cls_id, table] : addr_mapping) {
      for (auto [old_addr, new_addr] : table) {
        auto it = rfe_old_addr_to_rfr.find(old_addr);
        if (it == rfe_old_addr_to_rfr.end())
          continue;
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
    pool.clear();
    auto p = _dir / T::kClassName;
    assert(std::filesystem::exists(p));
    std::ifstream in_s{p, std::ios::binary};
    const std::size_t n_nodes = io::read_size(in_s);
    DEBUG("Begin loading pool of {}, {} node(s)", T::kClassName, n_nodes);
    pool.reserve(n_nodes);
    // DEBUG("Pool of {} prepared", T::kClassName);
    auto &table = addr_mapping[T::kClassID];
    table.resize(n_nodes);
    // DEBUG("Address mapping of {} prepared", T::kClassName);
    pool.for_each([&in_s, &table](std::size_t i, T &object) {
      DEBUG("Loading #{}, addr is {}", i, static_cast<void *>(&object));
      table[i].new_addr = &object;
      load_node(in_s, object);
    });
    DEBUG("End loading pool of {}", T::kClassName);
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
        break;
      default:
        break;
    }
  }

 private:
  std::filesystem::path _dir;
};
}  // namespace serde

#endif  // SERDE_DESERIALIZE__H
