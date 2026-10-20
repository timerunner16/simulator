/*
 *  vec2.cxx
 *
 *  impl for vec2
 */

#include "core/type/geom.hxx"
#include <algorithm>
#include <cmath>

namespace core {
Vec2::Vec2() : x{0}, y{0} {}

Vec2::Vec2(const f32 n) : x{n}, y{n} {}

Vec2::Vec2(const f32 x, const f32 y) : x{x}, y{y} {}

Vec2::Vec2(const Vec3 &obj) : x{obj.x}, y{obj.y} {}

Vec2::Vec2(const Vec4 &obj) : x{obj.x}, y{obj.y} {}

f32 Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

f32 Vec2::distance(const Vec2 &rhs) const {
    const f32 dx = x - rhs.x;
    const f32 dy = y - rhs.y;
    return std::sqrt(dx * dx + dy * dy);
}

f32 Vec2::dot(const Vec2 &rhs) const {
    return x * rhs.x + y * rhs.y;
}

Vec2 Vec2::normalize() const {
    return *this / length();
}

Vec2 Vec2::min(const Vec2 &rhs) const {
    return {std::min(x, rhs.x), std::min(y, rhs.y)};
}

Vec2 Vec2::min(const f32 rhs) const {
    return {std::min(x, rhs), std::min(y, rhs)};
}

Vec2 Vec2::max(const Vec2 &rhs) const {
    return {std::max(x, rhs.x), std::max(y, rhs.y)};
}

Vec2 Vec2::max(const f32 rhs) const {
    return {std::max(x, rhs), std::max(y, rhs)};
}

Vec2 Vec2::clamp(const Vec2 &minVal, const Vec2 &maxVal) const {
    return max(minVal).min(maxVal);
}

Vec2 Vec2::clamp(const f32 minVal, const f32 maxVal) const {
    return max(minVal).min(maxVal);
}

Vec2 Vec2::mix(const Vec2 &target, const Vec2 &wb) const {
    return *this * (Vec2(1.0) - wb) + target * wb;
}

Vec2 Vec2::mix(const Vec2 &target, const f32 wb) const {
    return *this * Vec2(1.0 - wb) + target * wb;
}

bool Vec2::operator==(const Vec2 &rhs) const {
    return x == rhs.x && y == rhs.y;
}

Vec2 &Vec2::operator+=(const Vec2 &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vec2 &Vec2::operator*=(const Vec2 &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vec2 &Vec2::operator/=(const Vec2 &rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

Vec2 &Vec2::operator*=(const f32 rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

Vec2 &Vec2::operator/=(const f32 rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator*(const Vec2 &rhs) const {
    return {x * rhs.x, y * rhs.y};
}

Vec2 operator*(const Vec2 &lhs, const f32 rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}

Vec2 operator*(const f32 lhs, const Vec2 &rhs) {
    return {lhs * rhs.x, lhs * rhs.y};
}

Vec2 Vec2::operator/(const Vec2 &rhs) const {
    return {x / rhs.x, y / rhs.y};
}

Vec2 Vec2::operator/(const f32 rhs) const {
    return {x / rhs, y / rhs};
}

Vec2 Vec2::operator-() const {
    return {-x, -y};
}
} // namespace core
