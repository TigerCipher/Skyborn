// ------------------------------------------------------------------------------
//
// Skyborn
//    Copyright 2023 Matthew Rogers
//
//    This library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation; either version 3 of the
//    License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, see <http://www.gnu.org/licenses/>.
//
// File Name: Maths.h
// Date File Created: 04/18/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Skyborn/Defines.h"

#include <format>

#ifndef SKY_USE_SIMD
    #define SKY_USE_SIMD 1
#endif

#if SKY_USE_SIMD
    #include <intrin.h>
    #include <xmmintrin.h>

    #define MAKE_SHUFFLE_MASK(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))

    // vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
    #define VEC_SWIZZLE_MASK(vec, mask)  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
    #define VEC_SWIZZLE(vec, x, y, z, w) VEC_SWIZZLE_MASK(vec, MAKE_SHUFFLE_MASK(x, y, z, w))
    #define VEC_SWIZZLE1(vec, x)         VEC_SWIZZLE_MASK(vec, MAKE_SHUFFLE_MASK(x, x, x, x))
    // special swizzle
    #define VEC_SWIZZLE_0022(vec) _mm_moveldup_ps(vec)
    #define VEC_SWIZZLE_1133(vec) _mm_movehdup_ps(vec)

    // return (vec1[x], vec1[y], vec2[z], vec2[w])
    #define VEC_SHUFFLE(vec1, vec2, x, y, z, w) _mm_shuffle_ps(vec1, vec2, MAKE_SHUFFLE_MASK(x, y, z, w))
    // special shuffle
    #define VEC_SHUFFLE_0101(vec1, vec2) _mm_movelh_ps(vec1, vec2)
    #define VEC_SHUFFLE_2323(vec1, vec2) _mm_movehl_ps(vec2, vec1)
#endif

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
constexpr f32 epsilon            = 1.192092896e-07f;
constexpr f32 infinity           = 1e30f;
constexpr f32 deg2rad_multiplier = pi / 180.0f;
constexpr f32 rad2deg_multiplier = 180.0f / pi;
constexpr f32 nan                = std::numeric_limits<f32>::quiet_NaN();

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

// standard abs is not constexpr until C++23 -- this engine is C++20
template<primitive_type T>
constexpr T abs(T x)
{
    return x < 0 ? -x : x;
}

constexpr f32 sqrt_newton(f32 x, f32 current, f32 previous)
{
    return current == previous ? current : sqrt_newton(x, 0.5f * (current + x / current), current);
}

constexpr f32 sqrt(f32 x)
{
    return x >= 0 && x < infinity ? sqrt_newton(x, x, 0.f) : nan;
}

constexpr bool near_equal(f32 x, f32 y, f32 tolerance = epsilon)
{
    return abs(x - y) <= tolerance;
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

    constexpr bool operator==(const vector2& other) const noexcept
    {
        return near_equal(x, other.x) && near_equal(y, other.y);
    }

    constexpr bool operator!=(const vector2& other) const noexcept
    {
        return !near_equal(x, other.x) || !near_equal(y, other.y);
    }

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }
};

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
        return near_equal(x, other.x) && near_equal(y, other.y) && near_equal(z, other.z);
    }

    constexpr bool operator!=(const vector3& other) const noexcept
    {
        return !near_equal(x, other.x) || !near_equal(y, other.y) || !near_equal(z, other.z);
    }

    // Casts to vec2
    constexpr explicit operator vector2() const { return vector2{ x, y }; }

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y + z * z; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }
};

class matrix
{
public:
    union
    {
        f32 m[16]{};
        f32 elements[4][4];
#if SKY_USE_SIMD
        alignas(16) __m128 data[4];
#endif
    };

    constexpr matrix() noexcept = default;

    // Can be used to quickly set up a matrix filled with zeros and then value at 00, 11, 22, 33 positions.
    // In other words, matrix{1.0f} creates an identity matrix
    /* constexpr*/ matrix(f32 value) noexcept
    {
        elements[0][0] = value;
        elements[1][1] = value;
        elements[2][2] = value;
        elements[3][3] = value;
    }

#if SKY_USE_SIMD
    friend matrix operator*(const matrix& a, const matrix& b) noexcept
    {
        matrix result{};
        __m128 row[4], col[4], tmp[4];

        // Transpose matrix b to facilitate column access
        matrix b_temp{ b };
        _MM_TRANSPOSE4_PS(b_temp.data[0], b_temp.data[1], b_temp.data[2], b_temp.data[3]);
        col[0] = b_temp.data[0];
        col[1] = b_temp.data[1];
        col[2] = b_temp.data[2];
        col[3] = b_temp.data[3];

        // Multiply each row of a by each column of b
        for (u32 i = 0; i < 4; ++i)
        {
            row[i]         = a.data[i];
            tmp[0]         = _mm_mul_ps(row[i], col[0]);
            tmp[1]         = _mm_mul_ps(row[i], col[1]);
            tmp[2]         = _mm_mul_ps(row[i], col[2]);
            tmp[3]         = _mm_mul_ps(row[i], col[3]);
            tmp[0]         = _mm_hadd_ps(tmp[0], tmp[1]);
            tmp[2]         = _mm_hadd_ps(tmp[2], tmp[3]);
            tmp[0]         = _mm_hadd_ps(tmp[0], tmp[2]);
            result.data[i] = tmp[0];
        }

        return result;
    }

    matrix& operator*=(const matrix& b) noexcept
    {
        __m128 row[4], col[4], tmp[4];

        // Transpose matrix b to facilitate column access
        matrix b_temp{ b };
        _MM_TRANSPOSE4_PS(b_temp.data[0], b_temp.data[1], b_temp.data[2], b_temp.data[3]);
        col[0] = b_temp.data[0];
        col[1] = b_temp.data[1];
        col[2] = b_temp.data[2];
        col[3] = b_temp.data[3];

        // Multiply each row of a by each column of b
        for (u32 i = 0; i < 4; ++i)
        {
            row[i]  = data[i];
            tmp[0]  = _mm_mul_ps(row[i], col[0]);
            tmp[1]  = _mm_mul_ps(row[i], col[1]);
            tmp[2]  = _mm_mul_ps(row[i], col[2]);
            tmp[3]  = _mm_mul_ps(row[i], col[3]);
            tmp[0]  = _mm_hadd_ps(tmp[0], tmp[1]);
            tmp[2]  = _mm_hadd_ps(tmp[2], tmp[3]);
            tmp[0]  = _mm_hadd_ps(tmp[0], tmp[2]);
            data[i] = tmp[0];
        }

        return *this;
    }

    // Inverses this matrix
    matrix& inverse() noexcept
    {
        // In large part based on this blog https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
        // use block matrix method
        // A is a matrix, then i(A) or iA means inverse of A, A# means adjugate of A, |A| is determinant, tr(A) is trace

        // sub matrices
        __m128 submatrix_a{ VEC_SHUFFLE_0101(data[0], data[1]) }; // A
        __m128 submatrix_b{ VEC_SHUFFLE_2323(data[0], data[1]) }; // B
        __m128 submatrix_c{ VEC_SHUFFLE_0101(data[2], data[3]) }; // C
        __m128 submatrix_d{ VEC_SHUFFLE_2323(data[2], data[3]) }; // D

        // determinant as (|A| |B| |C| |D|)
        __m128 det_sub{ _mm_sub_ps(
            _mm_mul_ps(VEC_SHUFFLE(data[0], data[2], 0, 2, 0, 2), VEC_SHUFFLE(data[1], data[3], 1, 3, 1, 3)),
            _mm_mul_ps(VEC_SHUFFLE(data[0], data[2], 1, 3, 1, 3), VEC_SHUFFLE(data[1], data[3], 0, 2, 0, 2))) };
        __m128 det_a{ VEC_SWIZZLE1(det_sub, 0) };
        __m128 det_b{ VEC_SWIZZLE1(det_sub, 1) };
        __m128 det_c{ VEC_SWIZZLE1(det_sub, 2) };
        __m128 det_d{ VEC_SWIZZLE1(det_sub, 3) };

        // let iM = 1/|M| * | X  Y |
        //                  | Z  W |

        // D#C
        __m128 d_c{ mat2_adj_mul(submatrix_d, submatrix_c) };
        // A#B
        __m128 a_b{ mat2_adj_mul(submatrix_a, submatrix_b) };
        // X# = |D|A - B(D#C)
        __m128 x_{ _mm_sub_ps(_mm_mul_ps(det_d, submatrix_a), mat2_mul(submatrix_b, d_c)) };
        // W# = |A|D - C(A#B)
        __m128 w_{ _mm_sub_ps(_mm_mul_ps(det_a, submatrix_d), mat2_mul(submatrix_c, a_b)) };

        // |M| = |A|*|D| + ...
        __m128 det_m{ _mm_mul_ps(det_a, det_d) };

        // Y# = |B|C - D(A#B)#
        __m128 y_{ _mm_sub_ps(_mm_mul_ps(det_b, submatrix_c), mat2_mul_adj(submatrix_d, a_b)) };
        // Z# = |C|B - A(D#C)#
        __m128 z_{ _mm_sub_ps(_mm_mul_ps(det_c, submatrix_b), mat2_mul_adj(submatrix_a, d_c)) };

        // |M| = |A|*|D| + |B|*|C| ...
        det_m = _mm_add_ps(det_m, _mm_mul_ps(det_b, det_c));

        // tr((A#B)(D#C))
        __m128 tr{ _mm_mul_ps(a_b, VEC_SWIZZLE(d_c, 0, 2, 1, 3)) };
        tr = _mm_hadd_ps(tr, tr);
        tr = _mm_hadd_ps(tr, tr);
        // |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
        det_m = _mm_sub_ps(det_m, tr);

        const __m128 adj_sign_mask{ _mm_setr_ps(1.f, -1.f, -1.f, 1.f) };
        // (1/|M|, -1/|M|, -1/|M|, 1/|M|)
        __m128 r_det_m{ _mm_div_ps(adj_sign_mask, det_m) };

        x_ = _mm_mul_ps(x_, r_det_m);
        y_ = _mm_mul_ps(y_, r_det_m);
        z_ = _mm_mul_ps(z_, r_det_m);
        w_ = _mm_mul_ps(w_, r_det_m);

        // apply adjugate and store, here we combine adjugate shuffle and store shuffle
        data[0] = VEC_SHUFFLE(x_, y_, 3, 1, 3, 1);
        data[1] = VEC_SHUFFLE(x_, y_, 2, 0, 2, 0);
        data[2] = VEC_SHUFFLE(z_, w_, 3, 1, 3, 1);
        data[3] = VEC_SHUFFLE(z_, w_, 2, 0, 2, 0);

        return *this;
    }
#else
    friend matrix operator*(const matrix& a, const matrix& b) noexcept
    {
        matrix out_matrix{ 1.0f };

        const f32* a_data{ a.m };
        f32*       dst{ out_matrix.m };
        for (u32 i = 0; i < 4; ++i)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                *dst = a_data[0] * b.m[0 + j] + //
                       a_data[1] * b.m[4 + j] + //
                       a_data[2] * b.m[8 + j] + //
                       a_data[3] * b.m[12 + j];
                ++dst;
            }
            a_data += 4;
        }

        return out_matrix;
    }

    // TODO: This function needs testing
    matrix& operator*=(const matrix& b) noexcept
    {
        matrix     temp{ *this };
        const f32* a_data{ temp.m };
        f32*       dst{ m };
        for (u32 i = 0; i < 4; ++i)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                *dst = a_data[0] * b.m[0 + j] + //
                       a_data[1] * b.m[4 + j] + //
                       a_data[2] * b.m[8 + j] + //
                       a_data[3] * b.m[12 + j];
                ++dst;
            }
            a_data += 4;
        }

        return *this;
    }

    constexpr matrix& inverse() noexcept
    {
        f32 t0  = m[10] * m[15];
        f32 t1  = m[14] * m[11];
        f32 t2  = m[6] * m[15];
        f32 t3  = m[14] * m[7];
        f32 t4  = m[6] * m[11];
        f32 t5  = m[10] * m[7];
        f32 t6  = m[2] * m[15];
        f32 t7  = m[14] * m[3];
        f32 t8  = m[2] * m[11];
        f32 t9  = m[10] * m[3];
        f32 t10 = m[2] * m[7];
        f32 t11 = m[6] * m[3];
        f32 t12 = m[8] * m[13];
        f32 t13 = m[12] * m[9];
        f32 t14 = m[4] * m[13];
        f32 t15 = m[12] * m[5];
        f32 t16 = m[4] * m[9];
        f32 t17 = m[8] * m[5];
        f32 t18 = m[0] * m[13];
        f32 t19 = m[12] * m[1];
        f32 t20 = m[0] * m[9];
        f32 t21 = m[8] * m[1];
        f32 t22 = m[0] * m[5];
        f32 t23 = m[4] * m[1];


        m[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
        m[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
        m[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
        m[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

        f32 d = 1.0f / (m[0] * m[0] + m[4] * m[1] + m[8] * m[2] + m[12] * m[3]);

        m[0]  = d * m[0];
        m[1]  = d * m[1];
        m[2]  = d * m[2];
        m[3]  = d * m[3];
        m[4]  = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
        m[5]  = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
        m[6]  = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
        m[7]  = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
        m[8]  = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
        m[9]  = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
        m[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
        m[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
        m[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
        m[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
        m[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
        m[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

        return *this;
    }
#endif

#if SKY_USE_SIMD
private:
    // 2x2 row major matrix multiply
    static __m128 mat2_mul(__m128 vec1, __m128 vec2) noexcept
    {
        return _mm_add_ps(_mm_mul_ps(vec1, VEC_SWIZZLE(vec2, 0, 3, 0, 3)),
                          _mm_mul_ps(VEC_SWIZZLE(vec1, 1, 0, 3, 2), VEC_SWIZZLE(vec2, 2, 1, 2, 1)));
    }

    // 2x2 row major matrix adjugate multiply
    static __m128 mat2_adj_mul(__m128 vec1, __m128 vec2) noexcept
    {
        return _mm_sub_ps(_mm_mul_ps(VEC_SWIZZLE(vec1, 3, 3, 0, 0), vec2),
                          _mm_mul_ps(VEC_SWIZZLE(vec1, 1, 1, 2, 2), VEC_SWIZZLE(vec2, 2, 3, 0, 1)));
    }

    // 2x2 row major matrix multiply adjugate
    static __m128 mat2_mul_adj(__m128 vec1, __m128 vec2) noexcept
    {
        return _mm_sub_ps(_mm_mul_ps(vec1, VEC_SWIZZLE(vec2, 3, 0, 3, 0)),
                          _mm_mul_ps(VEC_SWIZZLE(vec1, 1, 0, 3, 2), VEC_SWIZZLE(vec2, 2, 1, 2, 1)));
    }
#endif
};

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

    constexpr vector4() noexcept = default;

    // Creates identity vector (or vector with value at W)
    constexpr vector4(f32 value) noexcept : x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ value } {}

    constexpr vector4(f32 _x, f32 _y, f32 _z, f32 _w) noexcept : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}

    constexpr vector4(const vector3& vec, f32 _w) noexcept : x{ vec.x }, y{ vec.y }, z{ vec.z }, w{ _w } {}

#if SKY_USE_SIMD
    vector4(__m128 _data) noexcept : data{ _data } {}

    friend vector4 operator+(const vector4& a, const vector4& b) noexcept { return { _mm_add_ps(a.data, b.data) }; }

    friend vector4 operator-(const vector4& a, const vector4& b) noexcept { return { _mm_sub_ps(a.data, b.data) }; }

    friend vector4 operator*(const vector4& a, const vector4& b) noexcept { return { _mm_mul_ps(a.data, b.data) }; }

    friend vector4 operator*(const vector4& v, f32 scalar) noexcept
    {
        return { _mm_mul_ps(v.data, _mm_set1_ps(scalar)) };
    }

    friend vector4 operator*(f32 scalar, const vector4& v) noexcept
    {
        return { _mm_mul_ps(v.data, _mm_set1_ps(scalar)) };
    }

    friend vector4 operator/(const vector4& a, const vector4& b) noexcept { return { _mm_div_ps(a.data, b.data) }; }

    friend vector4 operator/(const vector4& v, f32 scalar) noexcept
    {
        return { _mm_div_ps(v.data, _mm_set1_ps(scalar)) };
    }

    vector4& operator+=(const vector4& right) noexcept
    {
        data = _mm_add_ps(data, right.data);
        return *this;
    }

    vector4& operator-=(const vector4& right) noexcept
    {
        data = _mm_sub_ps(data, right.data);
        return *this;
    }

    vector4& operator*=(const vector4& right) noexcept
    {
        data = _mm_mul_ps(data, right.data);
        return *this;
    }

    vector4& operator/=(const vector4& right) noexcept
    {
        data = _mm_div_ps(data, right.data);
        return *this;
    }

    vector4& operator+=(f32 scalar) noexcept
    {
        data = _mm_add_ps(data, _mm_set1_ps(scalar));
        return *this;
    }

    vector4& operator-=(f32 scalar) noexcept
    {
        data = _mm_sub_ps(data, _mm_set1_ps(scalar));
        return *this;
    }

    vector4& operator*=(f32 scalar) noexcept
    {
        data = _mm_mul_ps(data, _mm_set1_ps(scalar));
        return *this;
    }

    vector4& operator/=(f32 scalar) noexcept
    {
        data = _mm_div_ps(data, _mm_set1_ps(scalar));
        return *this;
    }

    [[nodiscard]] f32 length_squared() const noexcept
    {
        const __m128 product{ _mm_mul_ps(data, data) };
        const __m128 sum1{ _mm_add_ps(product, _mm_shuffle_ps(product, product, _MM_SHUFFLE(2, 3, 0, 1))) };
        const __m128 sum2{ _mm_add_ps(sum1, _mm_shuffle_ps(sum1, sum1, _MM_SHUFFLE(1, 0, 3, 2))) };

        f32 result;
        _mm_store_ss(&result, sum2);
        return result;
    }

    [[nodiscard]] f32 length() const noexcept
    {
        const __m128 product{ _mm_mul_ps(data, data) };
        const __m128 sum1{ _mm_add_ps(product, _mm_shuffle_ps(product, product, _MM_SHUFFLE(2, 3, 0, 1))) };
        const __m128 sum2{ _mm_add_ps(sum1, _mm_shuffle_ps(sum1, sum1, _MM_SHUFFLE(1, 0, 3, 2))) };
        const __m128 result{ _mm_sqrt_ss(sum2) };

        f32 length;
        _mm_store_ss(&length, result);
        return length;
    }

    [[nodiscard]] f32 normal() const noexcept { return length(); }
#else
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

    [[nodiscard]] constexpr f32 length_squared() const noexcept { return x * x + y * y + z * z + w * w; }

    [[nodiscard]] constexpr f32 length() const noexcept { return sqrt(length_squared()); }

    [[nodiscard]] constexpr f32 normal() const noexcept { return length(); }
#endif


    constexpr bool operator==(const vector4& other) const noexcept
    {
        return near_equal(x, other.x) && near_equal(y, other.y) && near_equal(z, other.z) && near_equal(w, other.w);
    }

    constexpr bool operator!=(const vector4& other) const noexcept
    {
        return !near_equal(x, other.x) || !near_equal(y, other.y) || !near_equal(z, other.z) || !near_equal(w, other.w);
    }

    constexpr explicit operator vector2() const { return vector2{ x, y }; }

    constexpr explicit operator vector3() const { return vector3{ x, y, z }; }
};

// Vector 2 functions

constexpr f32 distance(const vector2& first, const vector2& second)
{
    const vector2 dist{ first.x - second.x, first.y - second.y };
    return dist.length();
}

constexpr vector2 normalize(const vector2& vec)
{
    return vec / vec.length();
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

// Vector 3 functions

constexpr f32 distance(const vector3& a, const vector3& b)
{
    const vector3 dist{ a.x - b.x, a.y - b.y, a.z - b.z };
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

constexpr f32 dot(const vector4& a, const vector4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

constexpr vector3 normalize(const vector3& vec)
{
    return vec / vec.length();
}

constexpr vector3 forward(const matrix& mat)
{
    vector3 result{};
    result.x = -mat.m[2];
    result.y = -mat.m[6];
    result.z = -mat.m[10];
    result   = normalize(result);
    return result;
}

constexpr vector3 backward(const matrix& mat)
{
    vector3 result{};
    result.x = mat.m[2];
    result.y = mat.m[6];
    result.z = mat.m[10];
    result   = normalize(result);
    return result;
}

constexpr vector3 up(const matrix& mat)
{
    vector3 result{};
    result.x = mat.m[1];
    result.y = mat.m[5];
    result.z = mat.m[9];
    result   = normalize(result);
    return result;
}

constexpr vector3 down(const matrix& mat)
{
    vector3 result{};
    result.x = -mat.m[1];
    result.y = -mat.m[5];
    result.z = -mat.m[9];
    result   = normalize(result);
    return result;
}

constexpr vector3 left(const matrix& mat)
{
    vector3 result{};
    result.x = -mat.m[0];
    result.y = -mat.m[4];
    result.z = -mat.m[8];
    result   = normalize(result);
    return result;
}

constexpr vector3 right(const matrix& mat)
{
    vector3 result{};
    result.x = mat.m[0];
    result.y = mat.m[4];
    result.z = mat.m[8];
    result   = normalize(result);
    return result;
}

// Vector 4 functions

constexpr vector4 conjugate(const vector4& vec)
{
    return { -vec.x, -vec.y, -vec.z, vec.w };
}

#if SKY_USE_SIMD
inline vector4 lerp(const vector4& a, const vector4& b, f32 f)
{
    return a + f * (b - a);
}

inline vector4 normalize(const vector4& vec)
{
    const f32 len = vec.length();
    return { _mm_div_ps(vec.data, _mm_set1_ps(len)) };
}

inline vector4 inverse(const vector4& vec)
{
    return normalize(conjugate(vec));
}

#else
constexpr vector4 lerp(const vector4& a, const vector4& b, f32 f)
{
    return a + f * (b - a);
}

constexpr vector4 normalize(const vector4& vec)
{
    return vec / vec.length();
}

constexpr vector4 inverse(const vector4& vec)
{
    return normalize(conjugate(vec));
}
#endif

inline matrix as_matrix(const vector4& vec)
{
    matrix result{ 1.0f };
    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    const vector4 n{ normalize(vec) };

    result.m[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    result.m[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    result.m[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    result.m[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    result.m[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    result.m[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    result.m[8]  = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    result.m[9]  = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    result.m[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return result;
}

inline vector4 quat_from_axis_angle(const vector3& axis, f32 angle, bool should_normalize)
{
    const f32 half_angle = 0.5f * angle;
    const f32 s          = sinf(half_angle);
    const f32 c          = cosf(half_angle);

    vector4 q{ s * axis.x, s * axis.y, s * axis.z, c };
    if (should_normalize)
    {
        return normalize(q);
    }
    return q;
}

inline vector4 multiply_quaternion(const vector4& a, const vector4& b)
{
    vector4 result{};

    result.x = a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x;
    result.y = -a.x * b.z + a.y * b.w + a.z * b.x + a.w * b.y;
    result.z = a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z;
    result.w = -a.x * b.x - a.y * b.y - a.z * b.z + a.w * b.w;

    return result;
}

// Spherical linear interpolation
inline vector4 slerp(const vector4& a, const vector4& b, f32 percentage)
{
    // Normalize to avoid undefined behavior.
    vector4 v0{ normalize(a) };
    vector4 v1{ normalize(b) };

    // Compute the cosine of the angle between the two vectors.
    f32 dot_prod = dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot_prod < 0.0f)
    {
        v1.x     = -v1.x;
        v1.y     = -v1.y;
        v1.z     = -v1.z;
        v1.w     = -v1.w;
        dot_prod = -dot_prod;
    }

    constexpr f32 dot_threshold = 0.9995f;
    if (dot_prod > dot_threshold)
    {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        vector4 result{ v0.x + ((v1.x - v0.x) * percentage), v0.y + ((v1.y - v0.y) * percentage),
                        v0.z + ((v1.z - v0.z) * percentage), v0.w + ((v1.w - v0.w) * percentage) };

        return normalize(result);
    }

    // Since dot is in range [0, dot_threshold], acos is safe
    f32 theta_0{ acosf(dot_prod) };    // theta_0 = angle between input vectors
    f32 theta{ theta_0 * percentage }; // theta = angle between v0 and result
    f32 sin_theta{ sinf(theta) };      // compute this value only once
    f32 sin_theta_0{ sinf(theta_0) };  // compute this value only once

    f32 s0 = cosf(theta) - dot_prod * sin_theta / sin_theta_0; // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return { (v0.x * s0) + (v1.x * s1), (v0.y * s0) + (v1.y * s1), (v0.z * s0) + (v1.z * s1),
             (v0.w * s0) + (v1.w * s1) };
}

// Matrix functions

#if SKY_USE_SIMD
inline matrix inverse(const matrix& mat)
{
    matrix result{ mat };
    return result.inverse();
}
#else
constexpr matrix inverse(const matrix& mat)
{
    matrix result{ mat };
    return result.inverse();
}
#endif

inline matrix transpose(const matrix& mat) noexcept
{
    matrix result{ 1.0f };
    result.m[0]  = mat.m[0];
    result.m[1]  = mat.m[4];
    result.m[2]  = mat.m[8];
    result.m[3]  = mat.m[12];
    result.m[4]  = mat.m[1];
    result.m[5]  = mat.m[5];
    result.m[6]  = mat.m[9];
    result.m[7]  = mat.m[13];
    result.m[8]  = mat.m[2];
    result.m[9]  = mat.m[6];
    result.m[10] = mat.m[10];
    result.m[11] = mat.m[14];
    result.m[12] = mat.m[3];
    result.m[13] = mat.m[7];
    result.m[14] = mat.m[11];
    result.m[15] = mat.m[15];

    return result;
}

inline matrix perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip) noexcept
{
    matrix    result{};
    const f32 half_tan_fov = tanf(fov_radians * 0.5f);
    result.m[0]            = 1.0f / (aspect_ratio * half_tan_fov);
    result.m[5]            = 1.0f / half_tan_fov;
    result.m[10]           = -((far_clip + near_clip) / (far_clip - near_clip));
    result.m[11]           = -1.0f;
    result.m[14]           = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
    return result;
}

inline matrix orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip) noexcept
{
    matrix result{ 1.0f };

    const f32 lr = 1.0f / (left - right);
    const f32 bt = 1.0f / (bottom - top);
    const f32 nf = 1.0f / (near_clip - far_clip);

    result.m[0]  = -2.0f * lr;
    result.m[5]  = -2.0f * bt;
    result.m[10] = 2.0f * nf;

    result.m[12] = (left + right) * lr;
    result.m[13] = (top + bottom) * bt;
    result.m[14] = (far_clip + near_clip) * nf;

    return result;
}

inline matrix look_at(const vector3& position, const vector3& target, const vector3& up)
{
    matrix  result{};
    vector3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;
    z_axis   = normalize(z_axis);

    const vector3 x_axis{ normalize(cross(z_axis, up)) };
    const vector3 y_axis{ cross(x_axis, z_axis) };

    result.m[0]  = x_axis.x;
    result.m[1]  = y_axis.x;
    result.m[2]  = -z_axis.x;
    result.m[3]  = 0;
    result.m[4]  = x_axis.y;
    result.m[5]  = y_axis.y;
    result.m[6]  = -z_axis.y;
    result.m[7]  = 0;
    result.m[8]  = x_axis.z;
    result.m[9]  = y_axis.z;
    result.m[10] = -z_axis.z;
    result.m[11] = 0;
    result.m[12] = -dot(x_axis, position);
    result.m[13] = -dot(y_axis, position);
    result.m[14] = dot(z_axis, position);
    result.m[15] = 1.0f;

    return result;
}

inline matrix translation(const vector3& position)
{
    matrix result{ 1.0f };
    result.m[12] = position.x;
    result.m[13] = position.y;
    result.m[14] = position.z;
    return result;
}

inline matrix scale(const vector3& scale)
{
    matrix result{ 1.0f };
    result.m[0]  = scale.x;
    result.m[5]  = scale.y;
    result.m[10] = scale.z;
    return result;
}

inline matrix euler_x(f32 angle_radians)
{
    matrix    result{ 1.0f };
    const f32 c = cosf(angle_radians);
    const f32 s = sinf(angle_radians);

    result.m[5]  = c;
    result.m[6]  = s;
    result.m[9]  = -s;
    result.m[10] = c;
    return result;
}

inline matrix euler_y(f32 angle_radians)
{
    matrix    result{ 1.0f };
    const f32 c = cosf(angle_radians);
    const f32 s = sinf(angle_radians);

    result.m[0]  = c;
    result.m[2]  = -s;
    result.m[8]  = s;
    result.m[10] = c;
    return result;
}

inline matrix euler_z(f32 angle_radians)
{
    matrix    result{ 1.0f };
    const f32 c = cosf(angle_radians);
    const f32 s = sinf(angle_radians);

    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;
    return result;
}

inline matrix euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians)
{
    const matrix rx = euler_x(x_radians);
    const matrix ry = euler_y(y_radians);
    const matrix rz = euler_z(z_radians);
    matrix       result{ rx * ry };
    result *= rz;
    return result;
}

// TODO: SIMD version
// Calculates a rotation matrix based on the quaternion and the passed in center point.
inline matrix rotation(const vector4& q, const vector3& center)
{
    matrix result{};

    f32* o = result.m;
    o[0]   = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1]   = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2]   = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3]   = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8]  = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9]  = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    return result;
}

// Constants
constexpr vector2 zero_vec2  = {};
constexpr vector2 one_vec2   = { 1.0f };
constexpr vector2 up_vec2    = { 0.0f, 1.0f };
constexpr vector2 down_vec2  = { 0.0f, -1.0f };
constexpr vector2 right_vec2 = { 1.0f, 0.0f };
constexpr vector2 left_vec2  = { -1.0f, 0.0f };

constexpr vector3 zero_vec3    = {};
constexpr vector3 one_vec3     = { 1.0f };
constexpr vector3 up_vec3      = { 0.0f, 1.0f, 0.0f };
constexpr vector3 down_vec3    = { 0.0f, -1.0f, 0.0f };
constexpr vector3 right_vec3   = { 1.0f, 0.0f, 0.0f };
constexpr vector3 left_vec3    = { -1.0f, 0.0f, 0.0f };
constexpr vector3 forward_vec3 = { 0.0f, 0.0f, -1.0f };
constexpr vector3 back_vec3    = { 0.0f, 0.0f, 1.0f };

constexpr vector4 zero_vec4           = {};
constexpr vector4 one_vec4            = { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr vector4 identity_quaternion = { 1.0f };


} // namespace sky::math

// global namespace type aliases for convenience (though might move within sky::math)

using vec2 = sky::math::vector2;
using vec3 = sky::math::vector3;
using vec4 = sky::math::vector4;
using quat = sky::math::vector4;
using mat4 = sky::math::matrix;

// Formatting

template<>
struct std::formatter<vec2> : std::formatter<string_view>
{
    auto format(const vec2& obj, std::format_context& ctx)
    {
        std::string temp;
        std::format_to(std::back_inserter(temp), "<{:.4f}, {:.4f}>", obj.x, obj.y);

        return std::formatter<string_view>::format(temp, ctx);
    }
};

template<>
struct std::formatter<vec3> : std::formatter<string_view>
{
    auto format(const vec3& obj, std::format_context& ctx)
    {
        std::string temp;
        std::format_to(std::back_inserter(temp), "<");

        for (u32 i = 0; i < 2; ++i)
        {
            std::format_to(std::back_inserter(temp), "{:.4f}, ", obj.elements[i]);
        }

        std::format_to(std::back_inserter(temp), "{:.4f}>", obj.elements[2]);

        return std::formatter<string_view>::format(temp, ctx);
    }
};

template<>
struct std::formatter<vec4> : std::formatter<string_view>
{
    auto format(const vec4& obj, std::format_context& ctx)
    {
        std::string temp;
        std::format_to(std::back_inserter(temp), "<");

        for (u32 i = 0; i < 3; ++i)
        {
            std::format_to(std::back_inserter(temp), "{:.4f}, ", obj.elements[i]);
        }

        std::format_to(std::back_inserter(temp), "{:.4f}>", obj.elements[3]);

        return std::formatter<string_view>::format(temp, ctx);
    }
};

template<>
struct std::formatter<mat4> : std::formatter<string_view>
{
    auto format(const mat4& obj, std::format_context& ctx)
    {
        std::string temp;
        std::format_to(std::back_inserter(temp), "[ ");

        for (u32 i = 0; i < 4; ++i)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                if (j == 0)
                {
                    std::format_to(std::back_inserter(temp), "<{:.4f}, ", obj.elements[i][j]);
                } else if (j == 3)
                {
                    if (i != 3)
                    {
                        std::format_to(std::back_inserter(temp), "{:.4f}> // ", obj.elements[i][j]);
                    } else
                    {
                        std::format_to(std::back_inserter(temp), "{:.4f}> ", obj.elements[i][j]);
                    }
                } else
                {
                    std::format_to(std::back_inserter(temp), "{:.4f}, ", obj.elements[i][j]);
                }
            }
        }

        std::format_to(std::back_inserter(temp), " ]");

        return std::formatter<string_view>::format(temp, ctx);
    }
};