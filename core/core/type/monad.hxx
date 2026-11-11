/*
 *  core/type/monad.hxx
 *
 *  option and result types
 */

#ifndef simulator_core_type_monad_hxx
#define simulator_core_type_monad_hxx

#include <expected>
#include <functional>
#include <optional>
#include <variant>

namespace core {
template <class T> using Option = std::optional<T>;
template <class T> using OptionRef = std::optional<std::reference_wrapper<T>>;

template <class T, class E> using Result = std::expected<T, E>;

using Unit = std::monostate;
} // namespace core

#endif
