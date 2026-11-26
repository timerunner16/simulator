/*
 *  core/type/memory.hxx
 *
 *  smart pointer types
 */

#ifndef simulator_core_type_memory_hxx
#define simulator_core_type_memory_hxx

#include <memory>

namespace core {
template <class T> class Shared : public std::shared_ptr<T> {
  public:
    using std::shared_ptr<T>::shared_ptr;

    Shared(const std::shared_ptr<T> &ptr) : std::shared_ptr<T>(ptr) {}

    Shared(std::shared_ptr<T> &&ptr) : std::shared_ptr<T>(std::move(ptr)) {}

    template <class... Args> static auto create(Args &&...args) -> decltype(auto) {
        return std::make_shared<T>(std::forward<Args...>(args...));
    }
};
template <class T> using Weak = std::weak_ptr<T>;

template <class T, class Dp = std::default_delete<T>> class Unique : public std::unique_ptr<T> {
  public:
    using std::unique_ptr<T, Dp>::unique_ptr;

    Unique(std::unique_ptr<T, Dp> &&ptr) : std::unique_ptr<T, Dp>(std::move(ptr)) {}

    template <class... Args> static auto create(Args &&...args) -> decltype(auto) {
        return std::make_unique<T>(std::forward<Args...>(args...));
    }
};
} // namespace core

#endif
