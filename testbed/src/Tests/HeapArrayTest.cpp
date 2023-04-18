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
// File Name: HeapArrayTest.cpp
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "HeapArrayTest.h"

#include "TestManager.h"
#include "Expect.h"

#include <Skyborn/Util/HeapArray.h>

using namespace sky;

u8 only_init_once()
{
    utl::heap_array<u64> arr{ 30 };
    expect_should_be(30, arr.size());
    arr.initialize(50);
    expect_should_not_be(50, arr.size());

    return pass;
}

u8 default_init()
{
    utl::heap_array<u64> arr{ 5 };
    arr[0] = 43;
    expect_should_be(43, arr[0]);
    expect_should_be(0, arr[3]);

    utl::heap_array<const char*> names{ 4 };
    names[0] = "Elvis";
    expect_should_not_be(nullptr, names[0]);
    expect_should_be(nullptr, names[2]);

    return pass;
}

u8 keep_track_of_size_and_capacity()
{
    utl::heap_array<u64> arr{ 30 };

    expect_should_be(30, arr.size());
    u64 size = 30 * sizeof(u64);
    expect_should_be(size, arr.capacity());

    return pass;
}

void register_heap_array_tests()
{
    tests::register_test(only_init_once, "Heap array should only initialize once, and can never be resized");
    tests::register_test(default_init, "The Heap Array, once initialized, values may "
                                       "be set and read. If not set, should default to 0 (or null)");
    tests::register_test(keep_track_of_size_and_capacity, "Heap array should keep track of size and capacity");
}