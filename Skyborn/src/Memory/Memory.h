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
// File Name: Memory.h
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <string>

#include "Defines.h"


namespace sky
{
struct memory_tag
{
    enum tag : u32
    {
        unknown,
        array,
        darray,
        dictionary,
        ring_queue,
        bst,
        string,
        app,
        job,
        texture,
        material,
        renderer,
        game,
        entity,
        entity_node,
        scene,

        count
    };
};
namespace memory
{

//void increase_memory_count(size_t size);
//void decrease_memory_count(size_t size);


SAPI void initialize();
SAPI void shutdown();

// It's entirely possible there will be situations I don't just want to use smart pointers or new/delete. But rather than just pure C functions, I think
// it might be best to work towards custom allocators. Plus this way we can track memory leaks, easier than we could with new/delete
// But... in most cases I imagine I'll be using smart pointers


SAPI void* allocate(u64 size, memory_tag::tag tag);          // malloc
SAPI void  free(void* block, u64 size, memory_tag::tag tag); // free
SAPI void* zero_memory(void* block, u64 size);               // memset(block, 0, size)
SAPI void* copy(void* dest, const void* src, u64 size);      // memcpy
SAPI void* set(void* dest, i32 value, u64 size);             // memset

SAPI void* new_alloc(u64 size, memory_tag::tag tag);
SAPI void  del(void* block, u64 size, memory_tag::tag tag);

SAPI std::string get_usage_str();


template<typename T, memory_tag::tag Tag = memory_tag::unknown>
class ref_allocator
{
public:
    using value_type = T;

    template<typename Other>
    struct rebind
    {
        using other = ref_allocator<Other, Tag>;
    };

    ref_allocator() noexcept = default;

    template<typename Other>
    ref_allocator(const ref_allocator<Other, Tag>&) noexcept
    {}

    T* allocate(size_t size) { return (T*) new_alloc(size * sizeof(T), Tag); }

    void deallocate(T* ptr, size_t size) noexcept { del(ptr, sizeof(T) * size, Tag); }
};

template<typename T, typename Deleter, memory_tag::tag Tag = memory_tag::unknown>
class scope_allocator
{
public:
    using value_type = T;
    using pointer    = T*;

    template<typename Other>
    struct rebind
    {
        using other = scope_allocator<Other, Deleter, Tag>;
    };

    scope_allocator() noexcept = default;

    template<typename Other>
    scope_allocator(const scope_allocator<Other, Deleter, Tag>&) noexcept
    {}

    pointer allocate(size_t size) { return (pointer) new_alloc(size * sizeof(T), Tag); }

    void deallocate(pointer ptr, size_t size) noexcept { del(ptr, size * sizeof(T), Tag); }
};

template<typename T, memory_tag::tag Tag>
struct scope_deleter
{
    void operator()(T* ptr) const noexcept
    {
        scope_allocator<T, scope_deleter, Tag> alloc{};
        std::allocator_traits<decltype(alloc)>::destroy(alloc, ptr);
        alloc.deallocate(ptr, 1);
    }
};

} // namespace memory
} // namespace sky