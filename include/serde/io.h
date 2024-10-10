#ifndef SERDE_BYTES_IO__H
#define SERDE_BYTES_IO__H

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace serde::io {
namespace detail {
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void write(std::ostream &out, T value) {
  union {
    T i;
    char bytes[sizeof(T)];
  } u;
  u.i = value;
  out.write(u.bytes, sizeof(T));
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T read(std::istream &in) {
  union {
    T i;
    char bytes[sizeof(T)];
  } u;
  in.read(u.bytes, sizeof(T));
  return u.i;
}
}  // namespace detail

inline void write_u32(std::ostream &out, uint32_t value) {
  detail::write(out, value);
}
inline void write_u64(std::ostream &out, uint64_t value) {
  detail::write(out, value);
}
inline void write_size(std::ostream &out, std::size_t value) {
  detail::write(out, value);
}
template <typename T>
inline void write_ptr(std::ostream &out, T *ptr) {
  detail::write(out, reinterpret_cast<std::uintptr_t>(ptr));
}
inline void write_str(std::ostream &out, std::string_view s) {
  write_size(out, s.length());
  for (char c : s) {
    detail::write(out, c);
  }
}

inline uint32_t read_u32(std::istream &in) {
  return detail::read<uint32_t>(in);
}
inline uint64_t read_u64(std::istream &in) {
  return detail::read<uint64_t>(in);
}
inline std::size_t read_size(std::istream &in) {
  return detail::read<std::size_t>(in);
}
template <typename T>
inline T *read_ptr(std::istream &in) {
  return reinterpret_cast<T *>(detail::read<std::uintptr_t>(in));
}
inline std::string read_str(std::istream &in) {
  const size_t len = read_size(in);
  std::string ans(len, '\0');
  for (std::size_t i = 0; i < len; i++) {
    ans[i] = detail::read<char>(in);
  }
  return ans;
}
}  // namespace serde::io

#endif  // SERDE_BYTES_IO__H
