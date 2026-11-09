/*
 *  core/type/monad.hxx
 *
 *  option and result types
 */

#ifndef simulator_core_type_monad_hxx
#define simulator_core_type_monad_hxx

#include <expected>
#include <optional>

namespace core {
template <class T> using Option = std::optional<T>;

template <class T, class E> using Result = std::expected<T, E>;
} // namespace core

#endif
