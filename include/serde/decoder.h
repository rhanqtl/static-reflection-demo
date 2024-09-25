#ifndef SERDE_DECODER__H
#define SERDE_DECODER__H

#include <cstdint>
#include <istream>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "reflect/access.h"
#include "utility/logging.h"

extern std::unordered_map<std::uintptr_t, std::vector<std::pair<void *, void **>>>
    rfe_old_addr_to_rfr;

namespace serde::detail {
template <typename T, typename = void>
struct DataDecoder {
  void operator()(std::istream &in_stream, T &object) = delete;
};

template <typename T>
struct DataDecoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>> {
  void operator()(std::istream &in_stream, T &object) {
    using Access = reflect::Access<T>;
    if constexpr (Access::kHasSuper) load_as<typename Access::super_type>(object);
    load_fields(in_stream, object, std::make_index_sequence<Access::kNumFields>{});
  }

 private:
  template <typename U>
  bool load_as(U &object, std::istream &in_stream) {
    DataDecoder<U>{}(object, in_stream);
  }

  template <std::size_t... Is>
  bool load_fields(std::istream &in_stream, T &object, std::index_sequence<Is...>) {
    using Access = reflect::Access<T>;
    return (load_field<T, Is>(in_stream, object) && ...);
  }

  template <std::size_t I>
  bool load_field(std::istream &in_stream, const T &object) {
    using Access = reflect::Access<T>;
    using Field = typename Access::template FieldAt<I>;
    if constexpr (Field::is_transient) {
      // Do nothing.
    } else if constexpr (Field::is_static) {
      FATAL("Static fields are not supported yet");
    } else {
      DataDecoder<typename Field::type>(object.*Field::pointer);
    }
  }
};

template <typename T>
struct DataDecoder<T *> {
  void operator()(T *&ptr, std::istream &in_stream) {
    in_stream >> ptr;
    if constexpr (reflect::is_ast_node_v<T>) {
      rfe_old_addr_to_rfr[reinterpret_cast<std::uintptr_t>(ptr)].emplace_back(&ptr);
    }
  }
};

template <typename T>
struct DataDecoder<std::vector<T>> {
  using value_type = std::vector<T>;

  void operator()(value_type &xs, std::istream &in_stream) {
    std::size_t size;
    in_stream >> size;
    value_type(size).swap(xs);
    for (auto &x : xs) {
      DataDecoder<T>{}(in_stream, x);
    }
  }
};
}  // namespace serde::detail

#endif  // SERDE_DECODER__H
