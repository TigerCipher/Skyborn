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

#include <Skyborn/Memory/LinearAllocator.h>

using namespace sky;

u8 create_destroy()
{
    memory::linear_allocator alloc{ sizeof(u64) };

    expect_should_not_be(nullptr, alloc.memory());
    expect_should_be(sizeof(u64), alloc.total_size());
    expect_should_be(0, alloc.allocated());

    alloc.destroy();

    expect_should_be(nullptr, alloc.memory());
    expect_should_be(0, alloc.total_size());
    expect_should_be(0, alloc.allocated());

    return 1;
}

u8 single_allocation_all_space()
{
    memory::linear_allocator alloc{ sizeof(u64) };

    void* block{ alloc.allocate(sizeof(u64)) };

    expect_should_not_be(nullptr, block);
    expect_should_be(sizeof(u64), alloc.allocated());

    alloc.destroy();

    return 1;
}

u8 multiple_allocation_all_space()
{
    constexpr u64 max_allocs = 1024;

    memory::linear_allocator alloc{ sizeof(u64) * max_allocs };

    for (u64 i = 0; i < max_allocs; ++i)
    {
        void* block = alloc.allocate(sizeof(u64));

        expect_should_not_be(nullptr, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated());
    }

    alloc.destroy();

    return 1;
}

u8 multiple_allocation_overflow()
{
    constexpr u64            max_allocs = 3;
    memory::linear_allocator alloc{ sizeof(u64) * max_allocs };

    void* block;
    for (u64 i = 0; i < max_allocs; ++i)
    {
        block = alloc.allocate(sizeof(u64));

        expect_should_not_be(nullptr, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated());
    }

    LOG_DEBUG("Error is intentional in this test");

    // Should be trying to allocate passed the allotted space
    block = alloc.allocate(sizeof(u64));

    expect_should_be(nullptr, block);
    expect_should_be(sizeof(u64) * max_allocs, alloc.allocated());

    alloc.destroy();

    return 1;
}

u8 multple_allocation_all_space_then_free()
{
    constexpr u64 max_allocs = 1024;

    memory::linear_allocator alloc{ sizeof(u64) * max_allocs };

    for (u64 i = 0; i < max_allocs; ++i)
    {
        void* block = alloc.allocate(sizeof(u64));

        expect_should_not_be(nullptr, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated());
    }

    alloc.free_all();

    expect_should_be(0, alloc.allocated());

    alloc.destroy();

    return 1;
}

void register_linear_allocator_tests()
{
    test_manager::register_test(create_destroy, "Linear Allocator should be able to create and destroy");
    test_manager::register_test(single_allocation_all_space, "Linear Allocator single allocation covering all space");
    test_manager::register_test(multiple_allocation_all_space, "Linear Allocator multiple allocations for all space");
    test_manager::register_test(multiple_allocation_overflow, "Linear Allocator overflow");
    test_manager::register_test(multple_allocation_all_space_then_free, "Linear Allocator allocated should == 0 after free_all");
}
