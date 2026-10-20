/*
 *  core/type/float.hxx
 *
 *  float size types
 */

#ifndef simulator_core_type_float_hxx
#define simulator_core_type_float_hxx

namespace core {
using f32 = float;
using f64 = double;

inline f32 abs(const f32 n) {
    return n < 0 ? -n : n;
}

inline f64 abs(const f64 n) {
    return n < 0 ? -n : n;
}

inline bool near(const f32 a, const f32 b, const f32 epsilon) {
    return abs(a - b) < epsilon;
}
} // namespace core

#endif
