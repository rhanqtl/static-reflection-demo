#ifndef UTILITY_LOGGING__H
#define UTILITY_LOGGING__H

#include <iterator>
#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace utility::logging::detail {
template <typename... Args>
inline void do_logging(const char *file, int line, const char *level, const char *fmt,
                       Args &&...args) {
  std::string msg = fmt::format("{}:{} :: {} :: ", file, line, level);
  fmt::format_to(std::back_inserter(msg), fmt, std::forward<Args>(args)...);
  std::cout << msg << '\n';
  if (strcmp(level, "FATAL") == 0) std::abort();
}
}  // namespace utility::logging::detail

#define _CALL_LOGGING_FUNC(level, ...) \
  utility::logging::detail::do_logging(__FILE__, __LINE__, level, __VA_ARGS__)

#define DEBUG(...) _CALL_LOGGING_FUNC("DEBUG", __VA_ARGS__)
#define INFO(...) _CALL_LOGGING_FUNC("INFO", __VA_ARGS__)
#define FATAL(...) _CALL_LOGGING_FUNC("FATAL", __VA_ARGS__)

#endif  // UTILITY_LOGGING__H
