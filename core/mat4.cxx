/*
 *  mat2.cxx
 *
 *  impl for mat2
 */

#include "core/type/geom.hxx"

namespace core {
Mat4::Mat4() : cols{Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1)} {}

Mat4::Mat4(const f32 n)
    : cols{Vec4(n, 0, 0, 0), Vec4(0, n, 0, 0), Vec4(0, 0, n, 0), Vec4(0, 0, 0, n)} {}

// clang-format off
Mat4::Mat4(const f32 m11, const f32 m21, const f32 m31, const f32 m41,
		   const f32 m12, const f32 m22, const f32 m32, const f32 m42,
		   const f32 m13, const f32 m23, const f32 m33, const f32 m43,
           const f32 m14, const f32 m24, const f32 m34, const f32 m44)
	: cols{Vec4(m11, m21, m31, m41),
		   Vec4(m12, m22, m32, m42),
		   Vec4(m13, m23, m33, m43),
		   Vec4(m14, m24, m34, m44)} {}

// clang-format on

Mat4::Mat4(const Vec4 &c1, const Vec4 &c2, const Vec4 &c3, const Vec4 &c4) : cols{c1, c2, c3, c4} {}

Mat4::Mat4(const Mat2 &obj)
    : cols{Vec4(obj[0], 0, 0), Vec4(obj[1], 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1)} {}

Mat4::Mat4(const Mat3 &obj)
    : cols{Vec4(obj[0], 0), Vec4(obj[1], 0), Vec4(obj[2], 0), Vec4(0, 0, 0, 1)} {}

bool Mat4::operator==(const Mat4 &rhs) const {
    return cols[0] == rhs.cols[0] && cols[1] == rhs.cols[1] && cols[2] == rhs.cols[2] &&
           cols[3] == rhs.cols[3];
}

Mat4 Mat4::operator+(const Mat4 &rhs) const {
    return {cols[0] + rhs[0], cols[1] + rhs[1], cols[2] + rhs[2], cols[3] + rhs[3]};
}

Mat4 Mat4::operator-(const Mat4 &rhs) const {
    return {cols[0] - rhs[0], cols[1] - rhs[1], cols[2] - rhs[2], cols[3] - rhs[3]};
}

Mat4 Mat4::operator/(const Mat4 &rhs) const {
    return {cols[0] / rhs[0], cols[1] / rhs[1], cols[2] / rhs[2], cols[3] / rhs[3]};
}

Mat4 Mat4::operator/(const f32 rhs) const {
    return {cols[0] / rhs, cols[1] / rhs, cols[2] / rhs, cols[3] / rhs};
}

Mat4 operator*(const Mat4 &lhs, const Mat4 &rhs) {
    return {
        lhs[0] * rhs[0].x + lhs[1] * rhs[0].y + lhs[2] * rhs[0].z + lhs[3] * rhs[0].w,
        lhs[0] * rhs[1].x + lhs[1] * rhs[1].y + lhs[2] * rhs[1].z + lhs[3] * rhs[1].w,
        lhs[0] * rhs[2].x + lhs[1] * rhs[2].y + lhs[2] * rhs[2].z + lhs[3] * rhs[2].w,
        lhs[0] * rhs[3].x + lhs[1] * rhs[3].y + lhs[2] * rhs[3].z + lhs[3] * rhs[3].w,
    };
}

Vec4 operator*(const Mat4 &lhs, const Vec4 &rhs) {
    return lhs[0] * rhs.x + lhs[1] * rhs.y + lhs[2] * rhs.z + lhs[3] * rhs.w;
}

Vec4 operator*(const Vec4 &lhs, const Mat4 &rhs) {
    return {lhs.dot(rhs[0]), lhs.dot(rhs[1]), lhs.dot(rhs[2]), lhs.dot(rhs[3])};
}
} // namespace core
