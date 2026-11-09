/*
 *  core/type/memory.hxx
 *
 *  smart pointer types
 */

#ifndef simulator_core_type_monad_hxx
#define simulator_core_type_monad_hxx

#include <memory>

namespace core {
template <class T> using Shared = std::shared_ptr<T>;
template <class T> using Weak = std::weak_ptr<T>;
template <class T, class Dp = std::default_delete<T>> using Unique = std::unique_ptr<T, Dp>;
} // namespace core

#endif
