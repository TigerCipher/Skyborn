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
//  File Name: LinearAllocator.h
//  Date File Created: 04/09/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"

namespace sky::memory
{

class linear_allocator
{
public:
    SAPI constexpr linear_allocator() = default;

    SAPI explicit linear_allocator(u64 total_size, void* _memory = nullptr);

    SAPI ~linear_allocator();

    // Destructor will call this if needed. This is a public function mostly for the sake of early destruction if needed and for memory stats debug output
    SAPI void destroy();

    SAPI void* allocate(u64 size);

    SAPI void free_all();

    SAPI [[nodiscard]] constexpr void* memory() const { return m_memory; }

    SAPI [[nodiscard]] constexpr u64 total_size() const { return m_total_size; }

    SAPI [[nodiscard]] constexpr u64 allocated() const { return m_allocated; }

private:
    u64   m_total_size{};
    u64   m_allocated{};
    void* m_memory{nullptr};
    bool  m_owns_memory{};
};

} // namespace sky::memory