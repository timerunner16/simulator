/*
 *  core/type/int.hxx
 *
 *  integer size types
 */

#ifndef simulator_core_type_int_hxx
#define simulator_core_type_int_hxx

#include <cstddef>
#include <cstdint>

namespace core {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using usize = std::size_t;
} // namespace core

#endif
