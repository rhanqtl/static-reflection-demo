#ifndef SAVE_RESTORE__H
#define SAVE_RESTORE__H

#include <utility>

namespace utility::raii {
template <typename T>
class SaveRestore {
 public:
  SaveRestore(T &var, T &&new_value) : _var{var}, _old{std::move(var)} {
    _var = std::forward<T>(new_value);
  }
  ~SaveRestore() {
    _var = std::move(_old);
  }

 private:
  T &_var;
  T _old;
};
}  // namespace utility::raii

#define SAVE_RESTORE(var, new_value)          \
  ::utility::raii::SaveRestore _g##__LINE__ { \
    var, new_value                            \
  }

#endif  // SAVE_RESTORE__H
