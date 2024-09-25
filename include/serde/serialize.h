#ifndef SERDE_SERIALIZE__H
#define SERDE_SERIALIZE__H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast/ast_fwd.h"
#include "ast/decl.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/type.h"
#include "pool.h"
#include "reflect/access.h"
#include "serde/encoder.h"

namespace serde {
class ASTSaver {
 public:
  ASTSaver(const std::filesystem::path &dir) : _dir{dir} {}

  void save() {
    save_pools(std::make_index_sequence<std::tuple_size_v<ast::Nodes>>());

    auto index_stream = std::ofstream{_dir / "index.db"};
    for (const auto &[cls_id, xs] : _addr) {
      index_stream << cls_id;
      index_stream << xs.size();
      for (auto x : xs) {
        index_stream << x;
      }
    }
  }

 private:
  template <std::size_t... Is>
  void save_pools(std::index_sequence<Is...>) {
    (save_pool<std::tuple_element_t<Is, ast::Nodes>>(), ...);
  }

  template <typename T>
  void save_pool() {
    auto &pool = ast::Pool<T>::instance();
    DEBUG("Begin saving pool of {}, {} node(s)", T::kClassName, pool.num_nodes());
    auto p = _dir / T::kClassName;
    std::ofstream out_s{p, std::ios::binary};
    out_s << pool.num_nodes();
    auto &addr = _addr[T::kClassID];
    addr.resize(pool.num_nodes());
    pool.for_each([&out_s, &addr](std::size_t i, const T &node) {
      DEBUG("Saving #{}, addr is {}", i, static_cast<const void *>(&node));
      save_node(node, out_s);
      addr[i] = reinterpret_cast<std::uintptr_t>(&node);
    });
    DEBUG("End saving pool of {}", T::kClassName);
  }

  template <typename T>
  static void save_node(const T &node, std::ostream &out_s) {
    detail::DataEncoder<T>{}(out_s, node);
  }

 private:
  std::filesystem::path _dir;

  std::unordered_map</* Class ID */ int, std::vector<std::uintptr_t>> _addr;
};
}  // namespace serde

#endif  // SERDE_SERIALIZE__H
