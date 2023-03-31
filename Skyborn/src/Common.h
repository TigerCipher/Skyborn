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
//  File Name: CommonHeaders.h
//  Date File Created: 03/31/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Defines.h"
#include "Debug/Logger.h"
#include "Memory/Memory.h"

template<typename T, sky::memory_tag::tag Tag>
using ref = std::shared_ptr<T>;

template<typename T, sky::memory_tag::tag Tag, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr ref<T, Tag> create_ref(Args&&... args)
{
    return std::allocate_shared<T>(sky::memory::ref_allocator<T, Tag>{}, std::forward<Args>(args)...);
}

template<typename T, sky::memory_tag::tag Tag>
using scope = std::unique_ptr<T, sky::memory::scope_deleter<T, Tag>>;

template<typename T, typename Deleter, sky::memory_tag::tag Tag, typename... Args>
scope<T, Tag> allocate_unique(sky::memory::scope_allocator<T, Deleter, Tag> alloc, Args&&... args)
{
    auto ptr{ scope<T, Tag>(alloc.allocate(1), Deleter()) };
    if (ptr)
    {
        try
        {
            std::allocator_traits<sky::memory::scope_allocator<T, Deleter, Tag>>::construct(alloc, ptr.get(), std::forward<Args>(args)...);
            return ptr;
        } catch (...)
        {
            alloc.deallocate(ptr.release(), 1);
            throw;
        }
    }

    return nullptr;
}

template<typename T, sky::memory_tag::tag Tag, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr scope<T, Tag> create_scope(Args&&... args)
{
    return allocate_unique<T>(sky::memory::scope_allocator<T, sky::memory::scope_deleter<T, Tag>, Tag>{}, std::forward<Args>(args)...);
}

#define SCOPE_CAST(parent, tag, ptr) std::unique_ptr<parent, memory::scope_deleter<parent, tag>>{ ptr.release(), memory::scope_deleter<parent, tag>{} }