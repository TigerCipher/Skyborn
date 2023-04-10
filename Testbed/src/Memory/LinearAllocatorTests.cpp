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
//  File Name: LinearAllocatorTests.cpp
//  Date File Created: 04/09/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#include "LinearAllocatorTests.h"
#include "TestManager.h"
#include "Expect.h"

#include <Memory/LinearAllocator.h>

using namespace sky;

u8 linear_allocate_create_destroy()
{
    memory::linear_allocator alloc{sizeof(u64)};

    expect_should_not_be(nullptr, alloc.memory());
    expect_should_be(sizeof(u64), alloc.total_size());
    expect_should_be(0, alloc.allocated());

    alloc.destroy();

    expect_should_be(nullptr, alloc.memory());
    expect_should_be(0, alloc.total_size());
    expect_should_be(0, alloc.allocated());

    return 1;
}


void register_linear_allocator_tests()
{
    test_manager::register_test(linear_allocate_create_destroy, "Linear Allocator should be able to create and destroy");
}
