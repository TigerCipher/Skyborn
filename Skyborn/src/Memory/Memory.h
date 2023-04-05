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
#include "Debug/Logger.h"
#include "Platform/Platform.h"


namespace sky
{
struct memory_tag
{
    enum tag : u32
    {
        unknown,
        heap_array,
        vector,
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




void record_allocation(u64 size, memory_tag::tag tag);
void record_deallocation(u64 size, memory_tag::tag tag);

SAPI void initialize();
SAPI void shutdown();

// It's entirely possible there will be situations I don't just want to use smart pointers or new/delete. But rather than just pure C functions, I think
// it might be best to work towards custom allocators. Plus this way we can track memory leaks, easier than we could with new/delete
// But... in most cases I imagine I'll be using smart pointers


SAPI void* zero_memory(void* block, u64 size); // memset(block, 0, size)


// TODO: Replace old allocate usages with new one

template<typename T>
void allocate(T*& block, memory_tag::tag tag, u64 count = 1)
{
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::allocate was called using an unknown tag");
    }

    const u64 size{sizeof(T) * count};
    block = (T*)platform::allocate(size); // TODO: Alignment
    zero_memory(block, size);
    record_allocation(size, tag);
}

// TODO: Replace usages of old free with free_

template<typename T>
void free_(T* block, memory_tag::tag tag, u64 count = 1)
{
    const u64 size{sizeof(T) * count};
    if (!block || !size)
        return;
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::free was called using an unknown tag");
    }

    platform::free(block); // TODO: Alignment
    block = nullptr;

    record_deallocation(size, tag);
}

SAPI void* allocate(u64 size, memory_tag::tag tag);
SAPI void  free(void* block, u64 size, memory_tag::tag tag); // free

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

template<typename T, memory_tag::tag Tag>
class scope_allocator;

template<typename T, memory_tag::tag Tag = memory_tag::unknown>
struct scope_deleter
{
    void operator()(T* ptr) const noexcept
    {
        scope_allocator<T, Tag> alloc{};
        std::allocator_traits<decltype(alloc)>::destroy(alloc, ptr);
        alloc.deallocate(ptr, 1);
    }
};

template<typename T, memory_tag::tag Tag = memory_tag::unknown>
class scope_allocator
{
public:
    using value_type = T;
    using pointer    = T*;
    using deleter    = scope_deleter<T, Tag>;

    template<typename Other>
    struct rebind
    {
        using other = scope_allocator<Other, Tag>;
    };

    scope_allocator() noexcept = default;

    template<typename Other>
    scope_allocator(const scope_allocator<Other, Tag>&) noexcept
    {}

    pointer allocate(size_t size) { return (pointer) new_alloc(size * sizeof(T), Tag); }

    void deallocate(pointer ptr, size_t size) noexcept { del(ptr, size * sizeof(T), Tag); }
};


} // namespace memory
} // namespace sky


template<typename T, sky::memory_tag::tag Tag>
using ref = std::shared_ptr<T>;

template<typename T, sky::memory_tag::tag Tag, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr ref<T, Tag> create_ref(Args&&... args)
{
    return std::allocate_shared<T>(sky::memory::ref_allocator<T, Tag>{}, std::forward<Args>(args)...);
}

template<typename T, sky::memory_tag::tag Tag = sky::memory_tag::unknown>
using scope = std::unique_ptr<T, sky::memory::scope_deleter<T, Tag>>;

template<typename T, sky::memory_tag::tag Tag = sky::memory_tag::unknown, typename... Args>
scope<T, Tag> allocate_unique(sky::memory::scope_allocator<T, Tag> alloc, Args&&... args)
{
    auto ptr{ scope<T, Tag>(alloc.allocate(1), sky::memory::scope_allocator<T, Tag>::deleter()) };
    if (ptr)
    {
        try
        {
            std::allocator_traits<sky::memory::scope_allocator<T, Tag>>::construct(alloc, ptr.get(), std::forward<Args>(args)...);
            return ptr;
        } catch (...)
        {
            alloc.deallocate(ptr.release(), 1);
            throw;
        }
    }

    return nullptr;
}

template<typename T, sky::memory_tag::tag Tag = sky::memory_tag::unknown, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr scope<T, Tag> create_scope(Args&&... args)
{
    return allocate_unique<T>(sky::memory::scope_allocator<T, Tag>{}, std::forward<Args>(args)...);
}



#define SCOPE_CAST(parent, tag, ptr)                                                                                             \
    scope<parent, tag>                                                                                                           \
    {                                                                                                                            \
        ptr.release()                                                                                                            \
    }