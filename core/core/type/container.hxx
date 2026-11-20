/*
 *  core/type/container.hxx
 *
 *  container types
 */

#ifndef simulator_core_type_container_hxx
#define simulator_core_type_container_hxx

#include <array>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace core {
template <typename T, size_t N> using Array = std::array<T, N>;

template <typename T> using Vector = std::vector<T>;

template <typename T, size_t extent = std::dynamic_extent> using Span = std::span<T, extent>;

template <typename Kt, typename T> using Map = std::map<Kt, T>;

template <typename T1, typename T2> using Pair = std::pair<T1, T2>;

template <typename... T> using Tuple = std::tuple<T...>;

using String = std::string;

using StringView = std::string_view;
} // namespace core

#endif
