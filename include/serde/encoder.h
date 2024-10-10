#ifndef SERDE_ENCODER__H
#define SERDE_ENCODER__H

#include <iostream>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// #include "ast/decl.h"
#include "reflect/access.h"
#include "serde/io.h"
#include "utility/logging.h"

namespace serde::detail {
template <typename T, typename = void>
struct DataEncoder {
  void operator()(std::ostream &out_stream, const T &object) = delete;
};

template <typename T>
struct DataEncoder<const T> {
  void operator()(std::ostream &out_stream, const T &object);
};

template <typename T>
struct DataEncoder<T, std::enable_if_t<std::is_fundamental_v<T> && !std::is_const_v<T>>> {
  void operator()(std::ostream &out_stream, T object);
};

template <typename T>
struct DataEncoder<T, std::enable_if_t<std::is_enum_v<T>>> {
  void operator()(std::ostream &out_stream, T object) {
    using underlying_type = std::underlying_type_t<T>;
    DataEncoder<underlying_type>{}(out_stream, static_cast<underlying_type>(object));
  }
};

template <typename T>
struct DataEncoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>> {
  void operator()(std::ostream &out_stream, const T &object);

 private:
  template <typename U>
  void save_as(std::ostream &out_stream, const U &object);

  template <std::size_t... Is>
  void save_fields(std::ostream &out_stream, const T &object, std::index_sequence<Is...>);

  template <std::size_t I>
  void save_field(std::ostream &out_stream, const T &object);
};

template <typename T>
struct DataEncoder<T *> {
  void operator()(std::ostream &out_stream, const T *ptr);
};

template <typename T>
struct DataEncoder<std::vector<T>> {
  using value_type = std::vector<T>;

  void operator()(std::ostream &out_stream, const value_type &xs);
};

template <>
struct DataEncoder<std::string> {
  void operator()(std::ostream &out_stream, const std::string &s);
};

template <typename... Ts>
struct DataEncoder<std::tuple<Ts...>> {
  using value_type = std::tuple<Ts...>;

  void operator()(std::ostream &out_stream, const value_type &xs) {
    helper(std::make_index_sequence<std::tuple_size_v<value_type>>{}, out_stream, xs);
  }

  template <std::size_t... Is>
  void helper(std::index_sequence<Is...>, std::ostream &out_stream, const value_type &xs) {
    (DataEncoder<std::tuple_element_t<Is, value_type>>{}(out_stream, std::get<Is>(xs)), ...);
  }
};
}  // namespace serde::detail

namespace serde::detail {
template <typename T>
void DataEncoder<const T>::operator()(std::ostream &out_stream, const T &object) {
  DataEncoder<T>{}(out_stream, object);
}

template <typename T>
void DataEncoder<T, std::enable_if_t<std::is_fundamental_v<T> && !std::is_const_v<T>>>::operator()(
    std::ostream &out_stream, T object) {
  io::detail::write(out_stream, object);
}

template <typename T>
void DataEncoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>>::operator()(
    std::ostream &out_stream, const T &object) {
  using Access = reflect::Access<T>;
  if constexpr (Access::kHasSuper) save_as<typename Access::super_type>(out_stream, object);
  save_fields(out_stream, object, std::make_index_sequence<Access::kNumFields>{});
}

template <typename T>
template <typename U>
void DataEncoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>>::save_as(std::ostream &out_stream,
                                                                          const U &object) {
  DataEncoder<U>{}(out_stream, object);
}

template <typename T>
template <std::size_t... Is>
void DataEncoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>>::save_fields(
    std::ostream &out_stream, const T &object, std::index_sequence<Is...>) {
  using Access = reflect::Access<T>;
  (save_field<Is>(out_stream, object), ...);
}

template <typename T>
template <std::size_t I>
void DataEncoder<T, std::enable_if_t<reflect::is_ast_node_v<T>>>::save_field(
    std::ostream &out_stream, const T &object) {
  using Access = reflect::Access<T>;
  using Field = typename Access::template FieldAt<I>;
  if constexpr (Field::is_transient) {
    // Do nothing.
  } else if constexpr (Field::is_static) {
    FATAL("Static fields are not supported yet");
  } else {
    DataEncoder<typename Field::type>{}(out_stream, object.*Field::pointer);
  }
}

template <typename T>
void DataEncoder<T *>::operator()(std::ostream &out_stream, const T *ptr) {
  io::write_ptr(out_stream, ptr);
}

template <typename T>
void DataEncoder<std::vector<T>>::operator()(std::ostream &out_stream, const value_type &xs) {
  io::write_size(out_stream, xs.size());
  for (const auto &x : xs) {
    DataEncoder<T>{}(out_stream, x);
  }
}

inline void DataEncoder<std::string>::operator()(std::ostream &out_stream, const std::string &s) {
  io::write_str(out_stream, s);
}
}  // namespace serde::detail

#endif  // SERDE_ENCODER__H
