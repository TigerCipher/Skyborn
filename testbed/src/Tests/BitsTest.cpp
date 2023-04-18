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
// File Name: BitsTest.cpp
// Date File Created: 04/18/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "BitsTest.h"

#include "Expect.h"
#include "TestManager.h"

#include <Skyborn/Defines.h>
#include <Skyborn/Debug/Logger.h>

u8 set_correct_values_at_correct_bits()
{
    u8 number = 46; // 0010 1110
    LOG_DEBUG("Number: {} = {:0>8B}", number, number);
    u8 extracted = EXTRACT_BITS(number, 1, 4); // should be 1110 = 14
    expect_should_be(14, extracted);
    LOG_DEBUG("Extracted: {} = {:0>8B}", extracted, extracted);

    u8 num = 46;
    SET_BITS(num, 1, 4, 5); // 0010 1110 should become 0010 0101 = 37
    expect_should_be(37, num);
    LOG_DEBUG("Number: {} = {:0>8B}", num, num);
    u8 e = EXTRACT_BITS(num, 1, 4);
    expect_should_be(5, e);
    LOG_DEBUG("Extracted: {} = {:0>8B}", e, e);
    u8 e2 = EXTRACT_BITS(num, 5, 4);
    LOG_DEBUG("Extracted: {} = {:0>8B}", e2, e2);
    expect_should_be(2, e2);

    u8 num2 = 253;
    LOG_DEBUG("Num2: {} = {:0>8B}", num2, num2);
    u8 e3 = EXTRACT_BITS(num2, 1, 8);
    LOG_DEBUG("e3: {} = {:0>8B}", e3, e3);
    expect_should_be(253, e3);

    return pass;
}

#define MOUSE_X(data) EXTRACT_BITS((data), 1, 16);
#define MOUSE_Y(data) EXTRACT_BITS((data), 17, 16);

template<primitive_type T, primitive_type V>
constexpr void set_bits(T& number, const u8 p, const u8 n, const V value)
{
    // Need better way of passing 'value'
    // In example below, very annoying to have to do 377ui64, especially when it's intended as a u16
    T mask = ((1 << (n)) - 1) << ((p) -1);
    number &= ~mask;
    number |= ((value) << ((p) -1)) & mask;
}

// Silly me... all this bit work, but what if I want to pass floats or doubles as event data? Can't do that then
// Could make a union of say u64 and f64's... but to access floating point data as data.f64[0] and ints as MOUSE_X(data)
// Seems too silly and inconsistent

u8 new_event_data_idea()
{
    u64 data = 0;
    // SET_BITS(data, 1, 16, 377);
    u16 mouse_x = 377;
    set_bits(data, 1, 16, mouse_x); // intentional 'duplicate' of below
    set_bits(data, 1, 16, 377);     // intentional 'duplicate' of above
    LOG_DEBUG("LOW: {:0>16B}", 377u);
    LOG_DEBUG("Data: {:0>32B}", data);
    SET_BITS(data, 17, 16, 9325);
    LOG_DEBUG("HIGH: {:0>16B}", 9325u);
    LOG_DEBUG("Data: {:0>32B}", data);

    u16 mx = MOUSE_X(data);
    expect_should_be(377, mx);
    u16 my = MOUSE_Y(data);
    expect_should_be(9325, my);

    return pass;
}

void register_bits_tests()
{
    tests::register_test(set_correct_values_at_correct_bits, "Bits should be set and extracted correctly");
    tests::register_test(new_event_data_idea, "New event data tests");
}
