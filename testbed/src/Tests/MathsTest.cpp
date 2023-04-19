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
// File Name: MathsTest.cpp
// Date File Created: 04/18/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "MathsTest.h"

#include "Expect.h"
#include "TestManager.h"

using namespace sky;

u8 vector_mult_should_be_correct()
{
    {
        vec2 a{ 2.f, 2.f };
        vec2 b{ 3.f, 3.f };
        vec2 c = a * b;
        vec2 d{ 6.f, 6.f };
        expects_to_be_true(d == c);
    }
    {
        vec3 a{ 2.f, 2.f, 3.f };
        vec3 b{ 3.f, 3.f, 2.f };
        vec3 c = a * b;
        vec3 d{ 6.f, 6.f, 6.f };
        expects_to_be_true(d == c);
    }
    {
        vec4 a{ 2.f, 2.f, 3.f, 2.f };
        vec4 b{ 3.f, 3.f, 2.f, 3.f };
        vec4 c = a * b;
        vec4 d{ 6.f, 6.f, 6.f, 6.f };
        LOG_TRACE("Vector b: {}", b);
        expects_to_be_true(d == c);
    }
    return pass;
}

u8 matrix_mult()
{
    mat4 a{};
    a.m[0]  = 5;
    a.m[1]  = 7;
    a.m[2]  = 9;
    a.m[3]  = 10;
    a.m[4]  = 2;
    a.m[5]  = 3;
    a.m[6]  = 3;
    a.m[7]  = 8;
    a.m[8]  = 8;
    a.m[9]  = 10;
    a.m[10] = 2;
    a.m[11] = 3;
    a.m[12] = 3;
    a.m[13] = 3;
    a.m[14] = 4;
    a.m[15] = 8;

    mat4 b{};
    b.m[0]  = 3;
    b.m[1]  = 10;
    b.m[2]  = 12;
    b.m[3]  = 18;
    b.m[4]  = 12;
    b.m[5]  = 1;
    b.m[6]  = 3;
    b.m[7]  = 9;
    b.m[8]  = 9;
    b.m[9]  = 10;
    b.m[10] = 12;
    b.m[11] = 2;
    b.m[12] = 3;
    b.m[13] = 12;
    b.m[14] = 4;
    b.m[15] = 10;

    mat4 ab{};
    ab.m[0]  = 210;
    ab.m[1]  = 267;
    ab.m[2]  = 229;
    ab.m[3]  = 271;
    ab.m[4]  = 93;
    ab.m[5]  = 149;
    ab.m[6]  = 101;
    ab.m[7]  = 149;
    ab.m[8]  = 171;
    ab.m[9]  = 146;
    ab.m[10] = 162;
    ab.m[11] = 268;
    ab.m[12] = 105;
    ab.m[13] = 169;
    ab.m[14] = 125;
    ab.m[15] = 169;

    mat4 actual_ab = a * b;

    LOG_TRACE("actual_ab = {}", actual_ab);

    for (u32 i = 0; i < 16; ++i)
    {
        expect_float_to_equal(ab.m[i], actual_ab.m[i]);
    }

    return pass;
}

u8 matrix_inverse()
{
    mat4 a{};
    a.m[0] = 1;
    a.m[1] = 1;
    a.m[2] = 1;
    a.m[3] = 0;

    a.m[4] = 0;
    a.m[5] = 3;
    a.m[6] = 1;
    a.m[7] = 2;

    a.m[8]  = 1;
    a.m[9]  = 0;
    a.m[10] = 2;
    a.m[11] = 1;

    a.m[12] = 2;
    a.m[13] = 3;
    a.m[14] = 1;
    a.m[15] = 0;

    LOG_DEBUG("Before inverse: {}", a);

    mat4 inversed{};
    inversed.m[0]  = -3;
    inversed.m[1]  = -0.5f;
    inversed.m[2]  = 1;
    inversed.m[3]  = 1.5f;
    inversed.m[4]  = 1;
    inversed.m[5]  = 0.25f;
    inversed.m[6]  = -0.5f;
    inversed.m[7]  = -0.25f;
    inversed.m[8]  = 3;
    inversed.m[9]  = 0.25f;
    inversed.m[10] = -0.5f;
    inversed.m[11] = -1.25f;
    inversed.m[12] = -3;
    inversed.m[13] = 0;
    inversed.m[14] = 1;
    inversed.m[15] = 1;

    mat4 b = math::inverse(a);
    LOG_DEBUG("Inversed: {}", b);

    for (u32 i = 0; i < 16; ++i)
    {
        expect_float_to_equal(inversed.m[i], b.m[i]);
    }


    return pass;
}

void register_math_tests()
{
    tests::register_test(vector_mult_should_be_correct, "Vector multiplication should be correct");
    tests::register_test(matrix_mult, "Matrix should multiply correctly");
    tests::register_test(matrix_inverse, "Matrix should calculate its inverse correctly");
}
