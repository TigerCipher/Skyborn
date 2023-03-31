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

#include "Common.h"


namespace sky::memory
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

SAPI void initialize();
SAPI void shutdown();

// It's entirely possible there will be situations I don't just want to use smart pointers or new/delete. But rather than just pure C functions, I think
// it might be best to work towards custom allocators. Plus this way we can track memory leaks, easier than we could with new/delete
// But... in most cases I imagine I'll be using smart pointers

SAPI void* allocate(u64 size, memory_tag::tag tag);          // malloc
SAPI void  free(void* block, u64 size, memory_tag::tag tag); // free
SAPI void* zero_memory(void* block, u64 size);               // ZeroMemory
SAPI void* copy(void* dest, const void* src, u64 size);      // memcpy
SAPI void* set(void* dest, i32 value, u64 size);             // memset

SAPI std::string get_usage_str();

} // namespace sky::memory