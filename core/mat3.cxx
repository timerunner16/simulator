/*
 *  mat2.cxx
 *
 *  impl for mat2
 */

#include "core/type/geom.hxx"

namespace core {
Mat3::Mat3() : cols{Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)} {}

Mat3::Mat3(const f32 n) : cols{Vec3(n, 0, 0), Vec3(0, n, 0), Vec3(0, 0, n)} {}

// clang-format off
Mat3::Mat3(const f32 m11, const f32 m21, const f32 m31,
		   const f32 m12, const f32 m22, const f32 m32,
           const f32 m13, const f32 m23, const f32 m33)
	: cols{Vec3(m11, m21, m31),
		   Vec3(m12, m22, m32),
		   Vec3(m13, m23, m33)} {}

// clang-format on

Mat3::Mat3(const Vec3 &c1, const Vec3 &c2, const Vec3 &c3) : cols{c1, c2, c3} {}

Mat3::Mat3(const Mat2 &obj) : cols{Vec3(obj[0], 0), Vec3(obj[1], 0), Vec3(0, 0, 1)} {}

Mat3::Mat3(const Mat4 &obj) : cols{Vec3(obj[0]), Vec3(obj[1]), Vec3(obj[2])} {}

bool Mat3::operator==(const Mat3 &rhs) const {
    return cols[0] == rhs.cols[0] && cols[1] == rhs.cols[1] && cols[2] == rhs.cols[2];
}

Mat3 Mat3::operator+(const Mat3 &rhs) const {
    return {cols[0] + rhs[0], cols[1] + rhs[1], cols[2] + rhs[2]};
}

Mat3 Mat3::operator-(const Mat3 &rhs) const {
    return {cols[0] - rhs[0], cols[1] - rhs[1], cols[2] - rhs[2]};
}

Mat3 Mat3::operator/(const Mat3 &rhs) const {
    return {cols[0] / rhs[0], cols[1] / rhs[1], cols[2] / rhs[2]};
}

Mat3 Mat3::operator/(const f32 rhs) const {
    return {cols[0] / rhs, cols[1] / rhs, cols[2] / rhs};
}

Mat3 operator*(const Mat3 &lhs, const Mat3 &rhs) {
    return {
        lhs[0] * rhs[0].x + lhs[1] * rhs[0].y + lhs[2] * rhs[0].z,
        lhs[0] * rhs[1].x + lhs[1] * rhs[1].y + lhs[2] * rhs[1].z,
        lhs[0] * rhs[2].x + lhs[1] * rhs[2].y + lhs[2] * rhs[2].z,
    };
}

Vec3 operator*(const Mat3 &lhs, const Vec3 &rhs) {
    return {lhs[0] * rhs.x + lhs[1] * rhs.y + lhs[2] * rhs.z};
}

Vec3 operator*(const Vec3 &lhs, const Mat3 &rhs) {
    return {lhs.dot(rhs[0]), lhs.dot(rhs[1]), lhs.dot(rhs[2])};
}
} // namespace core
