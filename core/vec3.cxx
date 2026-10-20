/*
 *  vec3.cxx
 *
 *  impl for vec3
 */

#include "core/type/geom.hxx"
#include <algorithm>
#include <cmath>

namespace core {
Vec3::Vec3() : x{0}, y{0}, z{0} {}

Vec3::Vec3(const f32 n) : x{n}, y{n}, z{n} {}

Vec3::Vec3(const f32 x, const f32 y, const f32 z) : x{x}, y{y}, z{z} {}

Vec3::Vec3(const Vec2 &obj, const f32 z) : x{obj.x}, y{obj.y}, z{z} {}

Vec3::Vec3(const f32 x, const Vec2 &obj) : x{x}, y{obj.x}, z{obj.y} {}

Vec3::Vec3(const Vec4 &obj) : x{obj.x}, y{obj.y}, z{obj.z} {}

f32 Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

f32 Vec3::distance(const Vec3 &rhs) const {
    const f32 dx = x - rhs.x;
    const f32 dy = y - rhs.y;
    const f32 dz = z - rhs.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

f32 Vec3::dot(const Vec3 &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec3 Vec3::cross(const Vec3 &rhs) const {
    return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x};
}

Vec3 Vec3::normalize() const {
    return *this / length();
}

Vec3 Vec3::min(const Vec3 &rhs) const {
    return {std::min(x, rhs.x), std::min(y, rhs.y), std::min(z, rhs.z)};
}

Vec3 Vec3::min(const f32 rhs) const {
    return {std::min(x, rhs), std::min(y, rhs), std::min(z, rhs)};
}

Vec3 Vec3::max(const Vec3 &rhs) const {
    return {std::max(x, rhs.x), std::max(y, rhs.y), std::max(z, rhs.z)};
}

Vec3 Vec3::max(const f32 rhs) const {
    return {std::max(x, rhs), std::max(y, rhs), std::max(z, rhs)};
}

Vec3 Vec3::clamp(const Vec3 &minVal, const Vec3 &maxVal) const {
    return max(minVal).min(maxVal);
}

Vec3 Vec3::clamp(const f32 minVal, const f32 maxVal) const {
    return max(minVal).min(maxVal);
}

Vec3 Vec3::mix(const Vec3 &target, const Vec3 &wb) const {
    return *this * (Vec3(1.0) - wb) + target * wb;
}

Vec3 Vec3::mix(const Vec3 &target, const f32 wb) const {
    return *this * Vec3(1.0 - wb) + target * wb;
}

bool Vec3::operator==(const Vec3 &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

Vec3 &Vec3::operator+=(const Vec3 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vec3 &Vec3::operator*=(const Vec3 &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vec3 &Vec3::operator/=(const Vec3 &rhs) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

Vec3 &Vec3::operator*=(const f32 rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

Vec3 &Vec3::operator/=(const f32 rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

Vec3 Vec3::operator+(const Vec3 &rhs) const {
    return {x + rhs.x, y + rhs.y, z + rhs.z};
}

Vec3 Vec3::operator-(const Vec3 &rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
}

Vec3 Vec3::operator*(const Vec3 &rhs) const {
    return {x * rhs.x, y * rhs.y, z * rhs.z};
}

Vec3 operator*(const Vec3 &lhs, const f32 rhs) {
    return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
}

Vec3 operator*(const f32 lhs, const Vec3 &rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

Vec3 Vec3::operator/(const Vec3 &rhs) const {
    return {x / rhs.x, y / rhs.y, z / rhs.z};
}

Vec3 Vec3::operator/(const f32 rhs) const {
    return {x / rhs, y / rhs, z / rhs};
}

Vec3 Vec3::operator-() const {
    return {-x, -y, -z};
}
} // namespace core
