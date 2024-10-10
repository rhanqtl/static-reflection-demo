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

namespace utility::logging {
enum class Level { kFatal, kWarning, kInfo, kDebug };
inline auto level{Level::kInfo};

inline bool operator>=(Level lhs, Level rhs) {
  return static_cast<int>(lhs) >= static_cast<int>(rhs);
}
}  // namespace utility::logging

template <>
struct fmt::formatter<::utility::logging::Level> : formatter<string_view> {
  auto format(::utility::logging::Level v, format_context &ctx) const -> format_context::iterator {
    std::string_view name = "<unknown>";
    switch (v) {
      case utility::logging::Level::kFatal:
        name = "FATAL";
        break;
      case utility::logging::Level::kWarning:
        name = "WARNING";
        break;
      case utility::logging::Level::kInfo:
        name = "INFO";
        break;
      case utility::logging::Level::kDebug:
        name = "DEBUG";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

namespace utility::logging::detail {
template <typename... Args>
inline void do_logging(const char *file, int line, ::utility::logging::Level level, const char *fmt,
                       Args &&...args) {
  if (::utility::logging::level >= level) {
    std::string msg = fmt::format("{}:{} :: {} :: ", file, line, level);
    fmt::format_to(std::back_inserter(msg), fmt, std::forward<Args>(args)...);
    std::cout << msg << '\n';
    if (level == Level::kFatal) std::abort();
  }
}
}  // namespace utility::logging::detail

#define _CALL_LOGGING_FUNC(level, ...) \
  ::utility::logging::detail::do_logging(__FILE__, __LINE__, level, __VA_ARGS__)

#define DEBUG(...) _CALL_LOGGING_FUNC((::utility::logging::Level::kDebug), __VA_ARGS__)
#define INFO(...) _CALL_LOGGING_FUNC(::utility::logging::Level::kInfo, __VA_ARGS__)
#define FATAL(...) _CALL_LOGGING_FUNC(::utility::logging::Level::kFatal, __VA_ARGS__)

#endif  // UTILITY_LOGGING__H
