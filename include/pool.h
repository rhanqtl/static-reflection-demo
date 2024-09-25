#ifndef MEM_POOL__H
#define MEM_POOL__H

#include <cassert>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

namespace ast {
/// This implementation is not exception-safe, just for illustration.
template <typename T>
class Pool final {
 private:
  Pool() = default;
  Pool(const Pool &) = delete;
  Pool(Pool &&) = delete;
  Pool &operator=(const Pool &) = delete;
  Pool &operator=(Pool &&) = delete;
  ~Pool() = default;

 public:
  static Pool &instance() {
    static Pool singleton;
    return singleton;
  }

 public:
  template <typename... Args>
  T create(Args &&...args) {
    _data.emplace_front(std::forward<Args>(args)...);
    auto it = _data.begin();
    _index.emplace(std::addressof(*it), it);
  }

  void destroy(T *ptr) {
    auto it = _index.find(ptr);
    assert(it != _index.end());
    auto jt = it->second;
    _index.erase(it);
    _data.erase(jt);
  }

  template <typename F>
  void for_each(F &&func) const {
    std::size_t i = 0;
    for (auto &x : _data) {
      std::invoke(std::forward<F>(func), i, x);
      i++;
    }
  }

  void reserve(std::size_t n) {
    if (n > _data.size()) _data.resize(n);
  }

  std::size_t num_nodes() const {
    return _data.size();
  }

 private:
  /// A hash map keeping the insertion order of elements.
  std::list<T> _data;
  std::unordered_map<T *, decltype(_data.begin())> _index;
};
}  // namespace ast

#endif  // MEM_POOL__H
