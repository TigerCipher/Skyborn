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
// File Name: VectorTest.cpp
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VectorTest.h"
#include "TestManager.h"
#include "Expect.h"

#include <Skyborn/Util/Vector.h>

using namespace sky;

u8 push_and_erase()
{
    utl::vector<u64> vec{};
    expect_should_be(0, vec.size());

    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vec.push_back(6);

    expect_should_be(4, vec.size());

    vec.erase_unordered(3);

    expect_should_not_be(4, vec.size());

    return pass;
}

u8 correct_returns()
{
    utl::vector<u64> vec{};
    expects_to_be_true(vec.empty());
    expect_should_be(0, vec.capacity());
    expect_should_be(0, vec.size());
    vec.emplace_back();
    expects_to_be_false(vec.empty());

    vec.reserve(30);
    expect_should_be(30, vec.capacity());
    expect_should_be(1, vec.size());
    vec.clear();
    expect_should_be(0, vec.size());
    expect_should_not_be(0, vec.capacity());

    utl::vector<u32> vec2{ 50 };
    expect_should_be(50, vec2.capacity());
    expect_should_be(50, vec2.size());
    expects_to_be_false(vec2.empty());

    utl::vector<u32> vec3{};
    vec3.reserve(50);
    expect_should_be(50, vec3.capacity());
    expect_should_not_be(50, vec3.size());
    expects_to_be_true(vec3.empty());

    return pass;
}

void register_vector_tests()
{
    tests::register_test(push_and_erase, "Vector should keep track of it's size correctly");
    tests::register_test(correct_returns, "Vector should return correct values for empty, size, and capacity");
}