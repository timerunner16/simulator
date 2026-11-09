/*
 *  core/geom.hxx
 *
 *  geometric types
 */

#ifndef simulator_core_type_geom_hxx
#define simulator_core_type_geom_hxx

#include "float.hxx"
#include "int.hxx"

namespace core {
struct Vec2;
struct Vec3;
struct Vec4;

struct Mat2;
struct Mat3;
struct Mat4;

struct Vec2 {
    union {
        f32 raw[2];

        struct {
            f32 x;
            f32 y;
        };

        struct {
            f32 r;
            f32 g;
        };

        struct {
            f32 u;
            f32 v;
        };
    };

    f32 length() const;
    f32 distance(const Vec2 &rhs) const;
    f32 dot(const Vec2 &rhs) const;

    Vec2 normalize() const;

    Vec2 min(const Vec2 &rhs) const;
    Vec2 min(const f32 rhs) const;
    Vec2 max(const Vec2 &rhs) const;
    Vec2 max(const f32 rhs) const;
    Vec2 clamp(const Vec2 &minVal, const Vec2 &maxVal) const;
    Vec2 clamp(const f32 minVal, const f32 maxVal) const;
    Vec2 mix(const Vec2 &target, const Vec2 &wb) const;
    Vec2 mix(const Vec2 &target, const f32 wb) const;

    Vec2();
    Vec2(const f32 n);
    Vec2(const f32 x, const f32 y);
    Vec2(const Vec3 &obj);
    Vec2(const Vec4 &obj);

    bool operator==(const Vec2 &rhs) const;

    Vec2 &operator+=(const Vec2 &rhs);
    Vec2 &operator-=(const Vec2 &rhs);
    Vec2 &operator*=(const Vec2 &rhs);
    Vec2 &operator/=(const Vec2 &rhs);

    Vec2 &operator*=(const f32 rhs);
    Vec2 &operator/=(const f32 rhs);

    Vec2 operator+(const Vec2 &rhs) const;
    Vec2 operator-(const Vec2 &rhs) const;
    Vec2 operator*(const Vec2 &rhs) const;
    Vec2 operator/(const Vec2 &rhs) const;

    friend Vec2 operator*(const Vec2 &lhs, const f32 rhs);
    friend Vec2 operator*(const f32 lhs, const Vec2 &rhs);

    Vec2 operator/(const f32 rhs) const;

    Vec2 operator-() const;

    f32 &operator[](const usize i) {
        return raw[i];
    }

    const f32 &operator[](const usize i) const {
        return raw[i];
    }
};

struct Vec3 {
    union {
        f32 raw[3];

        struct {
            f32 x;
            f32 y;
            f32 z;
        };

        struct {
            f32 r;
            f32 g;
            f32 b;
        };
    };

    Vec3();
    Vec3(const f32 n);
    Vec3(const f32 x, const f32 y, const f32 z);
    Vec3(const Vec2 &obj, const f32 z);
    Vec3(const f32 x, const Vec2 &obj);
    Vec3(const Vec4 &obj);

    f32 length() const;
    f32 distance(const Vec3 &rhs) const;
    f32 dot(const Vec3 &rhs) const;

    Vec3 cross(const Vec3 &rhs) const;
    Vec3 normalize() const;

    Vec3 min(const Vec3 &rhs) const;
    Vec3 min(const f32 rhs) const;
    Vec3 max(const Vec3 &rhs) const;
    Vec3 max(const f32 rhs) const;
    Vec3 clamp(const Vec3 &minVal, const Vec3 &maxVal) const;
    Vec3 clamp(const f32 minVal, const f32 maxVal) const;
    Vec3 mix(const Vec3 &target, const Vec3 &wb) const;
    Vec3 mix(const Vec3 &target, const f32 wb) const;

    bool operator==(const Vec3 &rhs) const;

    Vec3 &operator+=(const Vec3 &rhs);
    Vec3 &operator-=(const Vec3 &rhs);
    Vec3 &operator*=(const Vec3 &rhs);
    Vec3 &operator/=(const Vec3 &rhs);

    Vec3 &operator*=(const f32 rhs);
    Vec3 &operator/=(const f32 rhs);

    Vec3 operator+(const Vec3 &rhs) const;
    Vec3 operator-(const Vec3 &rhs) const;
    Vec3 operator*(const Vec3 &rhs) const;
    Vec3 operator/(const Vec3 &rhs) const;

    friend Vec3 operator*(const Vec3 &lhs, const f32 rhs);
    friend Vec3 operator*(const f32 lhs, const Vec3 &rhs);

    Vec3 operator/(const f32 rhs) const;

    Vec3 operator-() const;

    f32 &operator[](const usize i) {
        return raw[i];
    }

    const f32 &operator[](const usize i) const {
        return raw[i];
    }
};

struct Vec4 {
    union {
        f32 raw[4];

        struct {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };

        struct {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };
    };

    Vec4();
    Vec4(const f32 n);
    Vec4(const f32 x, const f32 y, const f32 z, const f32 w);
    Vec4(const Vec2 &obj, const f32 z, const f32 w);
    Vec4(const f32 x, const Vec2 &obj, const f32 w);
    Vec4(const f32 x, const f32 y, const Vec2 &obj);
    Vec4(const Vec2 &lobj, const Vec2 &robj);
    Vec4(const Vec3 &obj, const f32 w);
    Vec4(const f32 x, const Vec3 &obj);

    f32 length() const;
    f32 distance(const Vec4 &rhs) const;
    f32 dot(const Vec4 &rhs) const;

    Vec4 normalize() const;

    Vec4 min(const Vec4 &rhs) const;
    Vec4 min(const f32 rhs) const;
    Vec4 max(const Vec4 &rhs) const;
    Vec4 max(const f32 rhs) const;
    Vec4 clamp(const Vec4 &minVal, const Vec4 &maxVal) const;
    Vec4 clamp(const f32 minVal, const f32 maxVal) const;
    Vec4 mix(const Vec4 &target, const Vec4 &wb) const;
    Vec4 mix(const Vec4 &target, const f32 wb) const;

    bool operator==(const Vec4 &rhs) const;

    Vec4 &operator+=(const Vec4 &rhs);
    Vec4 &operator-=(const Vec4 &rhs);
    Vec4 &operator*=(const Vec4 &rhs);
    Vec4 &operator/=(const Vec4 &rhs);

    Vec4 &operator*=(const f32 rhs);
    Vec4 &operator/=(const f32 rhs);

    Vec4 operator+(const Vec4 &rhs) const;
    Vec4 operator-(const Vec4 &rhs) const;
    Vec4 operator*(const Vec4 &rhs) const;
    Vec4 operator/(const Vec4 &rhs) const;

    friend Vec4 operator*(const Vec4 &lhs, const f32 rhs);
    friend Vec4 operator*(const f32 lhs, const Vec4 &rhs);

    Vec4 operator/(const f32 rhs) const;

    Vec4 operator-() const;

    f32 &operator[](const usize i) {
        return raw[i];
    }

    const f32 &operator[](const usize i) const {
        return raw[i];
    }
};

struct Mat2 {
    Vec2 cols[2];

    Mat2();
    Mat2(const f32 n);
    // clang-format off
    Mat2(const f32 m11, const f32 m21,
		 const f32 m12, const f32 m22);
    // clang-format on
    Mat2(const Vec2 &c1, const Vec2 &c2);
    Mat2(const Mat3 &m);
    Mat2(const Mat4 &m);

    bool operator==(const Mat2 &rhs) const;

    Mat2 operator+(const Mat2 &rhs) const;
    Mat2 operator-(const Mat2 &rhs) const;
    Mat2 operator/(const Mat2 &rhs) const;
    Mat2 operator/(const f32 rhs) const;

    friend Mat2 operator*(const Mat2 &lhs, const Mat2 &rhs);
    friend Vec2 operator*(const Mat2 &lhs, const Vec2 &rhs);
    friend Vec2 operator*(const Vec2 &lhs, const Mat2 &rhs);
    friend Mat2 operator*(const Mat2 &lhs, const f32 rhs);
    friend Mat2 operator*(const f32 lhs, const Mat2 &rhs);

    Vec2 &operator[](const usize i) {
        return cols[i];
    }

    const Vec2 &operator[](const usize i) const {
        return cols[i];
    }
};

struct Mat3 {
    Vec3 cols[3];

    Mat3();
    Mat3(const f32 n);
    // clang-format off
    Mat3(const f32 m11, const f32 m21, const f32 m31,
		 const f32 m12, const f32 m22, const f32 m32,
         const f32 m13, const f32 m23, const f32 m33);
    // clang-format on
    Mat3(const Vec3 &c1, const Vec3 &c2, const Vec3 &c3);
    Mat3(const Mat2 &m);
    Mat3(const Mat4 &m);

    bool operator==(const Mat3 &rhs) const;

    Mat3 operator+(const Mat3 &rhs) const;
    Mat3 operator-(const Mat3 &rhs) const;
    Mat3 operator/(const Mat3 &rhs) const;
    Mat3 operator/(const f32 rhs) const;

    friend Mat3 operator*(const Mat3 &lhs, const Mat3 &rhs);
    friend Vec3 operator*(const Mat3 &lhs, const Vec3 &rhs);
    friend Vec3 operator*(const Vec3 &lhs, const Mat3 &rhs);
    friend Mat3 operator*(const Mat3 &lhs, const f32 rhs);
    friend Mat3 operator*(const f32 lhs, const Mat3 &rhs);

    Vec3 &operator[](const usize i) {
        return cols[i];
    }

    const Vec3 &operator[](const usize i) const {
        return cols[i];
    }
};

struct Mat4 {
    Vec4 cols[4];

    Mat4();
    Mat4(const f32 n);
    // clang-format off
    Mat4(const f32 m11, const f32 m21, const f32 m31, const f32 m41,
		 const f32 m12, const f32 m22, const f32 m32, const f32 m42,
		 const f32 m13, const f32 m23, const f32 m33, const f32 m43,
         const f32 m14, const f32 m24, const f32 m34, const f32 m44);
    // clang-format on
    Mat4(const Vec4 &c1, const Vec4 &c2, const Vec4 &c3, const Vec4 &c4);
    Mat4(const Mat2 &m);
    Mat4(const Mat3 &m);

    bool operator==(const Mat4 &rhs) const;

    Mat4 operator+(const Mat4 &rhs) const;
    Mat4 operator-(const Mat4 &rhs) const;
    Mat4 operator/(const Mat4 &rhs) const;
    Mat4 operator/(const f32 rhs) const;

    friend Mat4 operator*(const Mat4 &lhs, const Mat4 &rhs);
    friend Vec4 operator*(const Mat4 &lhs, const Vec4 &rhs);
    friend Vec4 operator*(const Vec4 &lhs, const Mat4 &rhs);
    friend Mat4 operator*(const Mat4 &lhs, const f32 rhs);
    friend Mat4 operator*(const f32 lhs, const Mat4 &rhs);

    Vec4 &operator[](const usize i) {
        return cols[i];
    }

    const Vec4 &operator[](const usize i) const {
        return cols[i];
    }
};
} // namespace core

#endif
