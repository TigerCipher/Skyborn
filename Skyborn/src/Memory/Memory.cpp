﻿//  ------------------------------------------------------------------------------
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
//  File Name: Memory.cpp
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Memory.h"
#include "Debug/Logger.h"
#include "Platform/Platform.h"

namespace sky::memory
{

namespace
{

constexpr u64 gib{ 1_GB };
constexpr u64 mib{ 1_MB };
constexpr u64 kib{ 1_KB };

constexpr const char* tag_strings[memory_tag::count]{
    "UNKNOWN    ", "ARRAY      ", "DARRAY     ", "DICTIONARY ", "RING_QUEUE ", "BST        ", "STRING     ", "APP        ",
    "JOB        ", "TEXTURE    ", "MATERIAL   ", "RENDERER   ", "GAME       ", "ENTITY     ", "ENTITY_NODE", "SCENE      ",
};

static_assert(_countof(tag_strings) == memory_tag::count);

struct memory_stats
{
    u64 total_allocated{};
    u64 tagged_allocations[memory_tag::count]{};
} stats;

} // anonymous namespace

//void increase_memory_count(size_t size) {}
//void decrease_memory_count(size_t size) {}
void initialize() {}

void shutdown() {}


void* allocate(u64 size, memory_tag::tag tag)
{
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::allocate was called using an unknown tag");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    void* block{ platform::allocate(size) }; // TODO: Alignment

    zero_memory(block, size);

    LOG_TRACE(std::format("Allocated a block of {} bytes", size));

    return block;
}

void free(void* block, u64 size, memory_tag::tag tag)
{
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::free was called using an unknown tag");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    platform::free(block); // TODO: Alignment

    LOG_TRACE(std::format("Deallocated a block of {} bytes", size));
}

void* zero_memory(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* copy(void* dest, const void* src, u64 size)
{
    return memcpy(dest, src, size);
}

void* set(void* dest, i32 value, u64 size)
{
    return memset(dest, value, size);
}

std::string get_usage_str()
{
    std::string str{ "System memory use (tagged):\n" };

    u64 offset{ str.length() };
    for (u32 i{ 0 }; i < memory_tag::count; ++i)
    {
        char unit[4]{ "XiB" };
        f32  amount{ 1.0f };
        if (stats.tagged_allocations[i] >= gib)
        {
            unit[0] = 'G';
            amount  = (f32) stats.tagged_allocations[i] / (f32) gib;
        } else if (stats.tagged_allocations[i] >= mib)
        {
            unit[0] = 'M';
            amount  = (f32) stats.tagged_allocations[i] / (f32) mib;
        } else if (stats.tagged_allocations[i] >= kib)
        {
            unit[0] = 'K';
            amount  = (f32) stats.tagged_allocations[i] / (f32) kib;
        } else
        {
            unit[0] = 'B';
            unit[1] = '\0';
            amount  = (f32) stats.tagged_allocations[i];
        }

        str += std::format("  {}: {:.2f} {}\n", tag_strings[i], amount, unit);
    }

    return str;
}

} // namespace sky::memory
