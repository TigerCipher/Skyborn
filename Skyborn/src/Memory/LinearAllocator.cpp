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
//  File Name: LinearAllocator.cpp
//  Date File Created: 04/09/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "LinearAllocator.h"
#include "Memory.h"
#include "Debug/Logger.h"

namespace sky::memory
{

linear_allocator::linear_allocator(u64 total_size, void* _memory) : m_total_size{ total_size }
{
    m_owns_memory = _memory == nullptr;
    if (_memory)
    {
        m_memory = _memory;
    } else
    {
        m_memory = memory::allocate(total_size, memory_tag::linear_allocator);
    }
}

linear_allocator::~linear_allocator()
{
    // if total size is 0, then destroy has already been called as a means of early destruction
    if (m_total_size)
    {
        destroy();
    }
}

void linear_allocator::destroy()
{
    m_allocated = 0;
    if (m_owns_memory && m_memory)
    {
        free(m_memory, m_total_size, memory_tag::linear_allocator);
    }
    m_memory      = nullptr;
    m_total_size  = 0;
    m_owns_memory = false;
}

void* linear_allocator::allocate(u64 size)
{
    if (m_total_size && m_memory)
    {
        if (m_allocated + size > m_total_size)
        {
            u64 remaining{ m_total_size - m_allocated };
            LOG_FATALF("Linear allocator attempted to allocate {}B, but only {}B remaining", size, remaining);
            return nullptr;
        }

        void* block{ (u8*) m_memory + m_allocated };
        m_allocated += size;

        return block;
    }

    LOG_ERROR("Linear allocator not yet initialized");
    return nullptr;
}

void linear_allocator::free_all()
{
    if (m_total_size && m_memory)
    {
        m_allocated = 0;
        zero_memory(m_memory, m_total_size);
    }
}

} // namespace sky::memory
