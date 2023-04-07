//  ------------------------------------------------------------------------------
//
//  Skyborn
//     Copyright 2023 Matthew Rogers
//
//     This library is free software; you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as
//     published by the Free Software Foundation; either version 3 of the
//     License, or (at your option) any later version.
//
//     This library is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//     Lesser General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public
//     License along with this library; if not, see <http://www.gnu.org/licenses/>.
//
//  File Name: Math.h
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include <xmmintrin.h>

#include "Defines.h"

// TODO: Need to look into how to implement SIMD more before actually using it
#define SKY_USE_SIMD 0

namespace sky::math
{

constexpr f32 pi                 = 3.14159265358979323846f;
constexpr f32 two_pi             = pi * 2.0f;
constexpr f32 half_pi            = pi * 0.5f;
constexpr f32 qtr_pi             = pi * 0.25f;
constexpr f32 one_over_pi        = 1.0f / pi;
constexpr f32 one_over_two_pi    = 1.0f / two_pi;
constexpr f32 sqrt_two           = 1.41421356237309504880f;
constexpr f32 sqrt_three         = 1.73205080756887729352f;
constexpr f32 sqrt_half          = 0.70710678118654752440f;
constexpr f32 sqrt_third         = 0.57735026918962576450f;
constexpr f32 epsilon            = 1.192092896e-07f; // FLT_EPSILON from float.h -- smallest such that 1.0 + FLT_EPSILON != 1.0
constexpr f32 infinity           = std::numeric_limits<f32>::infinity();
constexpr f32 deg2rad_multiplier = pi / 180.0f;
constexpr f32 rad2deg_multiplier = 180.0f / pi;

template<primitive_type T>
constexpr T min(T a, T b)
{
    return a < b ? a : b;
}

template<primitive_type T>
constexpr T max(T a, T b)
{
    return a < b ? b : a;
}

template<primitive_type T>
constexpr T clamp(T value, T lower, T upper)
{
    return min(upper, max(lower, value));
}

// TODO: Needs testing
// More accurately to call this near_equal
constexpr bool is_equal(f32 x, f32 y, f32 tolerance = epsilon)
{
    return abs(x - y) <= tolerance; // * abs(x);
}

constexpr bool near_zero(f32 val, f32 tolerance = epsilon)
{
    return abs(val) <= tolerance;
}

constexpr f32 to_radians(f32 degrees)
{
    return degrees * deg2rad_multiplier;
}

constexpr f32 to_degrees(f32 radians)
{
    return radians * rad2deg_multiplier;
}

constexpr bool is_power_of_2(u64 value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

constexpr f32 lerp(f32 a, f32 b, f32 f)
{
    return a + f * (b - a);
}

class vector2
{
public:
    union
    {
        f32 elements[2]{};

        struct
        {
            union
            {
                f32 x, r, s, u;
            };

            union
            {
                f32 y, g, t, v;
            };
        };
    };

    constexpr vector2() noexcept : x{ 0 }, y{ 0 } {}

    constexpr vector2(f32 value) noexcept : x{ value }, y{ value } {}

    constexpr vector2(f32 _x, f32 _y) noexcept : x{ _x }, y{ _y } {}

    constexpr friend vector2 operator+(const vector2& a, const vector2& b) noexcept { return { a.x + b.x, a.y + b.y }; }

    constexpr friend vector2 operator-(const vector2& a, const vector2& b) noexcept { return { a.x - b.x, a.y - b.y }; }

    constexpr friend vector2 operator*(const vector2& a, const vector2& b) noexcept { return { a.x * b.x, a.y * b.y }; }

    constexpr friend vector2 operator*(const vector2& v, f32 scalar) noexcept { return { v.x * scalar, v.y * scalar }; }

    constexpr friend vector2 operator*(f32 scalar, const vector2& v) noexcept { return { v.x * scalar, v.y * scalar }; }

    constexpr friend vector2 operator/(const vector2& a, const vector2& b) noexcept { return { a.x / b.x, a.y / b.y }; }

    constexpr friend vector2 operator/(const vector2& v, f32 scalar) noexcept { return { v.x / scalar, v.y / scalar }; }

    constexpr vector2& operator+=(const vector2& right) noexcept
    {
        x += right.x;
        y += right.y;
        return *this;
    }

    constexpr vector2& operator-=(const vector2& right) noexcept
    {
        x -= right.x;
        y -= right.y;
        return *this;
    }

    constexpr vector2& operator*=(const vector2& right) noexcept
    {
        x *= right.x;
        y *= right.y;
        return *this;
    }

    constexpr vector2& operator/=(const vector2& right) noexcept
    {
        x /= right.x;
        y /= right.y;
        return *this;
    }

    constexpr vector2& operator+=(f32 scalar) noexcept
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    constexpr vector2& operator-=(f32 scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    constexpr vector2& operator*=(f32 scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr vector2& operator/=(f32 scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr bool operator==(const vector2& other) const noexcept { return is_equal(x, other.x) && is_equal(y, other.y); }

    constexpr bool operator!=(const vector2& other) const noexcept { return !is_equal(x, other.x) || !is_equal(y, other.y); }

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }

    // returns a copy of this vector, normalized
    [[nodiscard]] constexpr vector2 normalized() const noexcept
    {
        const f32 len{ length() };
        return { x / len, y / len };
    }

    // Normalizes this vector and returns it as a reference
    constexpr vector2& normalize() noexcept
    {
        const f32 len{ length() };
        x /= len;
        y /= len;
        return *this;
    }
};

constexpr f32 distance(const vector2& first, const vector2& second)
{
    const vector2 dist{ first.x - second.x, first.y - second.y };
    return dist.length();
}

constexpr vector2 lerp(const vector2& a, const vector2& b, f32 f)
{
    return a + f * (b - a);
}

constexpr f32 dot(const vector2& a, const vector2& b)
{
    return a.x * b.x + a.y * b.y;
}

// Reflects vector V around vector N (N is expected to be normalized)
// Returns a copy
constexpr vector2 reflect(const vector2& v, const vector2& n)
{
    return v - 2.0f * dot(v, n) * n;
}

constexpr vector2 zero_vec2  = {};
constexpr vector2 one_vec2   = { 1.0f };
constexpr vector2 up_vec2    = { 0.0f, 1.0f };
constexpr vector2 down_vec2  = { 0.0f, -1.0f };
constexpr vector2 right_vec2 = { 1.0f, 0.0f };
constexpr vector2 left_vec2  = { -1.0f, 0.0f };

class vector3
{
public:
    union
    {
        f32 elements[3]{};

        struct
        {
            union
            {
                f32 x, r, s, u;
            };

            union
            {
                f32 y, g, t, v;
            };

            union
            {
                f32 z, b, p, w;
            };
        };
    };

    constexpr vector3() noexcept : x{ 0 }, y{ 0 }, z{ 0 } {}

    constexpr vector3(f32 value) noexcept : x{ value }, y{ value }, z{ value } {}

    constexpr vector3(f32 _x, f32 _y, f32 _z) noexcept : x{ _x }, y{ _y }, z{ _z } {}

    constexpr vector3(const vector2& vec, f32 _z = 0) noexcept : x{ vec.x }, y{ vec.y }, z{ _z } {}

    constexpr friend vector3 operator+(const vector3& a, const vector3& b) noexcept
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    constexpr friend vector3 operator-(const vector3& a, const vector3& b) noexcept
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z };
    }

    constexpr friend vector3 operator*(const vector3& a, const vector3& b) noexcept
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z };
    }

    constexpr friend vector3 operator*(const vector3& v, f32 scalar) noexcept
    {
        return { v.x * scalar, v.y * scalar, v.z * scalar };
    }

    constexpr friend vector3 operator*(f32 scalar, const vector3& v) noexcept
    {
        return { v.x * scalar, v.y * scalar, v.z * scalar };
    }

    constexpr friend vector3 operator/(const vector3& a, const vector3& b) noexcept
    {
        return { a.x / b.x, a.y / b.y, a.z / b.z };
    }

    constexpr friend vector3 operator/(const vector3& v, f32 scalar) noexcept
    {
        return { v.x / scalar, v.y / scalar, v.z / scalar };
    }

    constexpr vector3& operator+=(const vector3& right) noexcept
    {
        x += right.x;
        y += right.y;
        z += right.z;
        return *this;
    }

    constexpr vector3& operator-=(const vector3& right) noexcept
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        return *this;
    }

    constexpr vector3& operator*=(const vector3& right) noexcept
    {
        x *= right.x;
        y *= right.y;
        z *= right.z;
        return *this;
    }

    constexpr vector3& operator/=(const vector3& right) noexcept
    {
        x /= right.x;
        y /= right.y;
        z /= right.z;
        return *this;
    }

    constexpr vector3& operator+=(f32 scalar) noexcept
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    constexpr vector3& operator-=(f32 scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    constexpr vector3& operator*=(f32 scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr vector3& operator/=(f32 scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    constexpr bool operator==(const vector3& other) const noexcept
    {
        return is_equal(x, other.x) && is_equal(y, other.y) && is_equal(z, other.z);
    }

    constexpr bool operator!=(const vector3& other) const noexcept
    {
        return !is_equal(x, other.x) || !is_equal(y, other.y) || !is_equal(z, other.z);
    }

    explicit constexpr operator vector2() const { return vector2{ x, y }; }

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y + z * z; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }

    // returns a copy of this vector, normalized
    [[nodiscard]] constexpr vector3 normalized() const noexcept
    {
        const f32 len{ length() };
        return { x / len, y / len, z / len };
    }

    // Normalizes this vector and returns it as a reference
    constexpr vector3& normalize() noexcept
    {
        const f32 len{ length() };
        x /= len;
        y /= len;
        z /= len;
        return *this;
    }
};

constexpr f32 distance(const vector3& a, const vector3& b)
{
    vector3 dist{ a.x - b.x, a.y - b.y, a.z - b.z };
    return dist.length();
}

constexpr f32 dot(const vector3& a, const vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr vector3 cross(const vector3& a, const vector3& b)
{
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

constexpr vector3 lerp(const vector3& a, const vector3& b, f32 f)
{
    return a + f * (b - a);
}

constexpr vector3 zero_vec3    = {};
constexpr vector3 one_vec3     = { 1.0f };
constexpr vector3 up_vec3      = { 0.0f, 1.0f, 0.0f };
constexpr vector3 down_vec3    = { 0.0f, -1.0f, 0.0f };
constexpr vector3 right_vec3   = { 1.0f, 0.0f, 0.0f };
constexpr vector3 left_vec3    = { -1.0f, 0.0f, 0.0f };
constexpr vector3 forward_vec3 = { 0.0f, 0.0f, -1.0f };
constexpr vector3 back_vec3    = { 0.0f, 0.0f, 1.0f };

class vector4
{
public:
    union
    {
#if SKY_USE_SIMD
        alignas(16) __m128 data;
#endif
        alignas(16) f32 elements[4]{};

        struct
        {
            union
            {
                f32 x, r, s;
            };

            union
            {
                f32 y, g, t;
            };

            union
            {
                f32 z, b, p;
            };

            union
            {
                f32 w, a, q;
            };
        };
    };

    constexpr vector4() noexcept : x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 } {}

    constexpr vector4(f32 value) noexcept : x{ value }, y{ value }, z{ value }, w{ value } {}

#if SKY_USE_SIMD
    constexpr vector4(f32 _x, f32 _y, f32 _z, f32 _w) noexcept : data{ __mm_setr_ps(x, y, z, w) } {}

    constexpr vector4(const vector3& vec, f32 _w = 0) noexcept : data{ __mm_setr_ps(vec.x, vec.y, vec.z, _w) } {}
#else
    constexpr vector4(f32 _x, f32 _y, f32 _z, f32 _w) noexcept : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}

    constexpr vector4(const vector3& vec, f32 _w) noexcept : x{ vec.x }, y{ vec.y }, z{ vec.z }, w{ _w } {}
#endif

    constexpr friend vector4 operator+(const vector4& a, const vector4& b) noexcept
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    constexpr friend vector4 operator-(const vector4& a, const vector4& b) noexcept
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    constexpr friend vector4 operator*(const vector4& a, const vector4& b) noexcept
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    }

    constexpr friend vector4 operator*(const vector4& v, f32 scalar) noexcept
    {
        return { v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar };
    }

    constexpr friend vector4 operator*(f32 scalar, const vector4& v) noexcept
    {
        return { v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar };
    }

    constexpr friend vector4 operator/(const vector4& a, const vector4& b) noexcept
    {
        return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
    }

    constexpr friend vector4 operator/(const vector4& v, f32 scalar) noexcept
    {
        return { v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar };
    }

    constexpr vector4& operator+=(const vector4& right) noexcept
    {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
        return *this;
    }

    constexpr vector4& operator-=(const vector4& right) noexcept
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
        return *this;
    }

    constexpr vector4& operator*=(const vector4& right) noexcept
    {
        x *= right.x;
        y *= right.y;
        z *= right.z;
        w *= right.w;
        return *this;
    }

    constexpr vector4& operator/=(const vector4& right) noexcept
    {
        x /= right.x;
        y /= right.y;
        z /= right.z;
        w /= right.w;
        return *this;
    }

    constexpr vector4& operator+=(f32 scalar) noexcept
    {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    constexpr vector4& operator-=(f32 scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    constexpr vector4& operator*=(f32 scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr vector4& operator/=(f32 scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    constexpr bool operator==(const vector4& other) const noexcept
    {
        return is_equal(x, other.x) && is_equal(y, other.y) && is_equal(z, other.z) && is_equal(w, other.w);
    }

    constexpr bool operator!=(const vector4& other) const noexcept
    {
        return !is_equal(x, other.x) || !is_equal(y, other.y) || !is_equal(z, other.z) || !is_equal(w, other.w);
    }

    explicit constexpr operator vector2() const { return vector2{ x, y }; }

    explicit constexpr operator vector3() const { return vector3{ x, y, z }; }

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y + z * z + w * w; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }

    // returns a copy of this vector, normalized
    [[nodiscard]] constexpr vector4 normalized() const noexcept
    {
        const f32 len{ length() };
        return { x / len, y / len, z / len, w / len };
    }

    // Normalizes this vector and returns it as a reference
    constexpr vector4& normalize() noexcept
    {
        const f32 len{ length() };
        x /= len;
        y /= len;
        z /= len;
        w /= len;
        return *this;
    }
};

constexpr f32 dot(const vector4& a, const vector4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

constexpr vector4 lerp(const vector4& a, const vector4& b, f32 f)
{
    return a + f * (b - a);
}

constexpr vector4 zero_vec4 = {};
constexpr vector4 one_vec4  = { 1.0f };

} // namespace sky::math

// global namespace type aliases for convenience (though might move within sky::math)

using vec2 = sky::math::vector2;
using vec3 = sky::math::vector3;
using vec4 = sky::math::vector4;
using quat = vec4;
