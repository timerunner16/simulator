/*
 *  vec3.cxx
 *
 *  impl for vec3
 */

#include "core/type/geom.hxx"
#include <algorithm>
#include <cmath>

namespace core {
Vec4::Vec4() : x{0}, y{0}, z{0}, w{0} {}

Vec4::Vec4(const f32 n) : x{n}, y{n}, z{n}, w{n} {}

Vec4::Vec4(const f32 x, const f32 y, const f32 z, const f32 w) : x{x}, y{y}, z{z}, w{w} {}

Vec4::Vec4(const Vec2 &obj, const f32 z, const f32 w) : x{obj.x}, y{obj.y}, z{z}, w{w} {}

Vec4::Vec4(const f32 x, const Vec2 &obj, const f32 w) : x{x}, y{obj.x}, z{obj.y}, w{w} {}

Vec4::Vec4(const f32 x, const f32 y, const Vec2 &obj) : x{x}, y{y}, z{obj.x}, w{obj.y} {}

Vec4::Vec4(const Vec2 &lobj, const Vec2 &robj) : x{lobj.x}, y{lobj.y}, z{robj.x}, w{robj.y} {}

Vec4::Vec4(const Vec3 &obj, const f32 w) : x{obj.x}, y{obj.y}, z{obj.z}, w{w} {}

Vec4::Vec4(const f32 x, const Vec3 &obj) : x{x}, y{obj.x}, z{obj.y}, w{obj.z} {}

f32 Vec4::length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

f32 Vec4::distance(const Vec4 &rhs) const {
    const f32 dx = x - rhs.x;
    const f32 dy = y - rhs.y;
    const f32 dz = z - rhs.z;
    const f32 dw = w - rhs.w;
    return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

f32 Vec4::dot(const Vec4 &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Vec4 Vec4::normalize() const {
    return *this / length();
}

Vec4 Vec4::min(const Vec4 &rhs) const {
    return {std::min(x, rhs.x), std::min(y, rhs.y), std::min(z, rhs.z), std::min(w, rhs.w)};
}

Vec4 Vec4::min(const f32 rhs) const {
    return {std::min(x, rhs), std::min(y, rhs), std::min(z, rhs), std::min(w, rhs)};
}

Vec4 Vec4::max(const Vec4 &rhs) const {
    return {std::max(x, rhs.x), std::max(y, rhs.y), std::max(z, rhs.z), std::max(w, rhs.w)};
}

Vec4 Vec4::max(const f32 rhs) const {
    return {std::max(x, rhs), std::max(y, rhs), std::max(z, rhs), std::max(w, rhs)};
}

Vec4 Vec4::clamp(const Vec4 &minVal, const Vec4 &maxVal) const {
    return max(minVal).min(maxVal);
}

Vec4 Vec4::clamp(const f32 minVal, const f32 maxVal) const {
    return max(minVal).min(maxVal);
}

Vec4 Vec4::mix(const Vec4 &target, const Vec4 &wb) const {
    return *this * (Vec4(1.0) - wb) + target * wb;
}

Vec4 Vec4::mix(const Vec4 &target, const f32 wb) const {
    return *this * Vec4(1.0 - wb) + target * wb;
}

bool Vec4::operator==(const Vec4 &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

Vec4 &Vec4::operator+=(const Vec4 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vec4 &Vec4::operator*=(const Vec4 &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vec4 &Vec4::operator/=(const Vec4 &rhs) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

Vec4 &Vec4::operator*=(const f32 rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

Vec4 &Vec4::operator/=(const f32 rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

Vec4 Vec4::operator+(const Vec4 &rhs) const {
    return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
}

Vec4 Vec4::operator-(const Vec4 &rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
}

Vec4 Vec4::operator*(const Vec4 &rhs) const {
    return {x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
}

Vec4 operator*(const Vec4 &lhs, const f32 rhs) {
    return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs};
}

Vec4 operator*(const f32 lhs, const Vec4 &rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w};
}

Vec4 Vec4::operator/(const Vec4 &rhs) const {
    return {x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w};
}

Vec4 Vec4::operator/(const f32 rhs) const {
    return {x / rhs, y / rhs, z / rhs, w / rhs};
}

Vec4 Vec4::operator-() const {
    return {-x, -y, -z, -w};
}
} // namespace core
