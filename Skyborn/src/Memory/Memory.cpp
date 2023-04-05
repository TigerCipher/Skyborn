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

struct memory_stats
{
    u64 total_allocated{};
    u64 tagged_allocations[memory_tag::count]{};
    u64 allocations{};
} stats;

// clang-format off
constexpr const char* tag_strings[memory_tag::count]{
    "UNKNOWN    ",
    "ARRAY      ",
    "VECTOR     ",
    "DICTIONARY ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APP        ",
    "JOB        ",
    "TEXTURE    ", 
    "MATERIAL   ", 
    "RENDERER   ", 
    "GAME       ", 
    "ENTITY     ", 
    "ENTITY_NODE", 
    "SCENE      ",
};
// clang-format on

static_assert(_countof(tag_strings) == memory_tag::count);


} // anonymous namespace

void record_allocation(u64 size, memory_tag::tag tag)
{
    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;
    ++stats.allocations;

    LOG_TRACEF("Allocated a block of {} bytes, tagged as {} [total allocations: {}]", size, tag_strings[tag], stats.allocations);
}

void record_deallocation(u64 size, memory_tag::tag tag)
{
    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;
    --stats.allocations;
    LOG_TRACEF("Deallocated a block of {} bytes, tagged as {} [total allocations: {}]", size, tag_strings[tag],
               stats.allocations);
}

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

    void* block{ platform::allocate(size) }; // TODO: Alignment

    zero_memory(block, size);

    record_allocation(size, tag);
    return block;
}

void free(void* block, u64 size, memory_tag::tag tag)
{
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

void* new_alloc(u64 size, memory_tag::tag tag)
{
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::new_alloc was called using an unknown tag");
    }

    void* block{ operator new(size) };
    zero_memory(block, size);

    record_allocation(size, tag);
    return block;
}

void del(void* block, u64 size, memory_tag::tag tag)
{
    if (!block || !size)
        return;
    if (tag == memory_tag::unknown)
    {
        LOG_WARN("memory::del was called using an unknown tag");
    }


    operator delete(block);
    block = nullptr;

    record_deallocation(size, tag);
}

std::string get_usage_str()
{
    std::string str{ std::format("System memory use (tagged):\n{} Total Allocations\n", stats.allocations) };

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

    char      unit[4]{ "XiB" };
    const u64 total{ stats.total_allocated };
    f32       amount;
    if (total >= gib)
    {
        unit[0] = 'G';
        amount  = (f32) total / (f32) gib;
    } else if (total >= mib)
    {
        unit[0] = 'M';
        amount  = (f32) total / (f32) mib;
    } else if (total >= kib)
    {
        unit[0] = 'K';
        amount  = (f32) total / (f32) kib;
    } else
    {
        unit[0] = 'B';
        unit[1] = '\0';
        amount  = (f32) total;
    }

    str += std::format("Total: {:.2f} {}\n", amount, unit);

    return str;
}

} // namespace sky::memory
