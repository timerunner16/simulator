/*
 *  mat2.cxx
 *
 *  impl for mat2
 */

#include "core/type/geom.hxx"

namespace core {
Mat2::Mat2() : cols{Vec2(1, 0), Vec2(0, 1)} {}

Mat2::Mat2(const f32 n) : cols{Vec2(n, 0), Vec2(0, n)} {}

Mat2::Mat2(const f32 m11, const f32 m21, const f32 m12, const f32 m22)
    : cols{Vec2(m11, m21), Vec2(m12, m22)} {}

Mat2::Mat2(const Vec2 &c1, const Vec2 &c2) : cols{c1, c2} {}

Mat2::Mat2(const Mat3 &obj) : cols{Vec2(obj[0]), Vec2(obj[1])} {}

Mat2::Mat2(const Mat4 &obj) : cols{Vec2(obj[0]), Vec2(obj[1])} {}

bool Mat2::operator==(const Mat2 &rhs) const {
    return cols[0] == rhs.cols[0] && cols[1] == rhs.cols[1];
}

Mat2 Mat2::operator+(const Mat2 &rhs) const {
    return {cols[0] + rhs[0], cols[1] + rhs[1]};
}

Mat2 Mat2::operator-(const Mat2 &rhs) const {
    return {cols[0] - rhs[0], cols[1] - rhs[1]};
}

Mat2 Mat2::operator/(const Mat2 &rhs) const {
    return {cols[0] / rhs[0], cols[1] / rhs[1]};
}

Mat2 Mat2::operator/(const f32 rhs) const {
    return {cols[0] / rhs, cols[1] / rhs};
}

Mat2 operator*(const Mat2 &lhs, const Mat2 &rhs) {
    return {
        {lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1],
         lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1]},
        {lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1],
         lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1]},
    };
}

Vec2 operator*(const Mat2 &lhs, const Vec2 &rhs) {
    return {
        lhs[0][0] * rhs.x + lhs[1][0] * rhs.y,
        lhs[0][1] * rhs.x + lhs[1][1] * rhs.y,
    };
}

Vec2 operator*(const Vec2 &lhs, const Mat2 &rhs) {
    return {
        lhs.x * rhs[0][0] + lhs.y * rhs[1][0],
        lhs.x * rhs[0][1] + lhs.y * rhs[1][1],
    };
}

Mat2 operator*(const Mat2 &lhs, const f32 rhs) {
    return {lhs[0] * rhs, lhs[1] * rhs};
}

Mat2 operator*(const f32 lhs, const Mat2 &rhs) {
    return {lhs * rhs[0], lhs * rhs[1]};
}
} // namespace core
