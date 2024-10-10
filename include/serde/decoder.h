#ifndef SERDE_DECODER__H
#define SERDE_DECODER__H

#include <cstdint>
#include <istream>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "ast/api/pretty_print.h"
#include "ast/decl.h"
#include "ast/type.h"
#include "reflect/access.h"
#include "serde/io.h"
#include "utility/logging.h"
#include "utility/save_restore.h"

extern std::unordered_map<std::uintptr_t, std::vector<std::pair<void *, void **>>>
    rfe_old_addr_to_rfr;

static void *curr_ast_node{nullptr};

namespace serde::detail {
template <typename T, typename = void>
struct DataDecoder {
  void operator()(std::istream &in_stream, T &object) = delete;
};

template <typename T>
struct DataDecoder<const T> {
  void operator()(std::istream &in_stream, T &object) {
    DataDecoder<T>{}(in_stream, object);
  }
};

template <typename T>
struct DataDecoder<T, std::enable_if_t<std::is_fundamental_v<T> && !std::is_const_v<T>>> {
  void operator()(std::istream &in_stream, T &object) {
    object = io::detail::read<T>(in_stream);
  }
};

template <typename T>
struct DataDecoder<T, std::enable_if_t<std::is_enum_v<T>>> {
  void operator()(std::istream &in_stream, T &object) {
    using underlying_type = std::underlying_type_t<T>;
    DataDecoder<underlying_type>{}(in_stream, reinterpret_cast<underlying_type &>(object));
  }
};

template <typename T>
struct DataDecoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>> {
  void operator()(std::istream &in_stream, T &object) {
    using Access = reflect::Access<T>;
    SAVE_RESTORE(curr_ast_node, static_cast<void *>(&object));
    if constexpr (Access::kHasSuper)
      load_as<typename Access::super_type>(in_stream, object);
    // DEBUG("{} {} fields to load", __PRETTY_FUNCTION__, Access::kNumFields);
    load_fields(in_stream, object, std::make_index_sequence<Access::kNumFields>{});
    // DEBUG("[{}] {} fields loaded", T::kClassName, Access::kNumFields);
  }

 private:
  template <typename U>
  void load_as(std::istream &in_stream, U &object) {
    DataDecoder<U>{}(in_stream, object);
  }

  template <std::size_t... Is>
  void load_fields(std::istream &in_stream, T &object, std::index_sequence<Is...>) {
    using Access = reflect::Access<T>;
    (load_field<Is>(in_stream, object), ...);
  }

  template <std::size_t I>
  void load_field(std::istream &in_stream, T &object) {
    using Access = reflect::Access<T>;
    using Field = typename Access::template FieldAt<I>;
    // DEBUG("Loading {}-th field {}", I + 1, T::kFieldNames[I]);
    if constexpr (Field::is_transient) {
      // Do nothing.
    } else if constexpr (Field::is_static) {
      FATAL("Static fields are not supported yet");
    } else {
      DataDecoder<typename Field::type>{}(in_stream, object.*Field::pointer);
    }
  }
};

template <typename T>
struct DataDecoder<T *> {
  void operator()(std::istream &in_stream, T *&ptr) {
    auto old_addr = io::read_ptr<T>(in_stream);
    ptr = nullptr;
    if constexpr (reflect::is_ast_node_v<T>) {
      rfe_old_addr_to_rfr[reinterpret_cast<std::uintptr_t>(old_addr)].emplace_back(
          curr_ast_node, reinterpret_cast<void **>(&ptr));
    }
  }
};

template <typename T>
struct DataDecoder<std::vector<T>> {
  using value_type = std::vector<T>;

  void operator()(std::istream &in_stream, value_type &xs) {
    const std::size_t size = io::read_size(in_stream);
    value_type(size).swap(xs);
    for (std::size_t i = 0; i < size; i++) {
      DataDecoder<T>{}(in_stream, xs[i]);
    }
    // for (auto &x : xs) {
    //   DataDecoder<T>{}(in_stream, x);
    // }
  }
};

template <>
struct DataDecoder<std::string> {
  void operator()(std::istream &in_stream, std::string &s) {
    s = io::read_str(in_stream);
  }
};

template <typename... Ts>
struct DataDecoder<std::tuple<Ts...>> {
  using value_type = std::tuple<Ts...>;

  void operator()(std::istream &in_stream, value_type &xs) {
    helper(std::make_index_sequence<std::tuple_size_v<value_type>>{}, in_stream, xs);
  }

  template <std::size_t... Is>
  void helper(std::index_sequence<Is...>, std::istream &in_stream, value_type &xs) {
    (DataDecoder<std::tuple_element_t<Is, value_type>>{}(in_stream, std::get<Is>(xs)), ...);
  }
};
}  // namespace serde::detail

#endif  // SERDE_DECODER__H
