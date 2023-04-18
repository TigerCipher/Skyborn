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
// File Name: HeapArray.h
// Date File Created: 04/17/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Skyborn/Defines.h"

#include "Skyborn/Debug/Asserts.h"

namespace sky::utl
{
template<typename T>
class heap_array
{
public:
    constexpr heap_array() = default;

    constexpr explicit heap_array(u64 size) { initialize(size); }

    constexpr ~heap_array() { destroy(); }

    // copy ctor
    constexpr heap_array(const heap_array& o) { *this = o; }

    // move ctor
    constexpr heap_array(heap_array&& o) noexcept : m_size{ o.m_size }, m_data{ o.m_data } { o.reset(); }

    constexpr heap_array& operator=(const heap_array& o)
    {
        sky_assert(this != std::addressof(o));
        if (this != std::addressof(o))
        {
            destroy();
            initialize(o.m_size);
            for (u64 i = 0; i < o.m_size; ++i)
            {
                m_data[i] = o.m_data[i];
            }
        }

        return *this;
    }

    constexpr heap_array& operator=(heap_array&& o) noexcept
    {
        sky_assert(this != std::addressof(o));
        if (this != std::addressof(o))
        {
            destroy();
            move(o);
        }

        return *this;
    }

    constexpr void initialize(u64 count)
    {
        if (m_size)
            return;
        m_size = count;
        m_data = (T*) malloc(count * sizeof(T));
        memset(m_data, 0, count * sizeof(T));
    }

    constexpr void initialize(u64 count, const T& value)
    {
        sky_assert(!m_size);
        m_size = count;
        m_data = (T*) malloc(count * sizeof(T));
        for (u32 i = 0; i < count; ++i)
        {
            m_data[i] = value;
        }
    }

    [[nodiscard]] constexpr u64 size() const { return m_size; }

    [[nodiscard]] constexpr u64 capacity() const { return m_size * sizeof(T); }

    [[nodiscard]] constexpr T* data() { return m_data; }

    [[nodiscard]] constexpr T* data() const { return m_data; }

    constexpr T& operator[](u64 index)
    {
        sky_assert(m_data && index < m_size);
        return m_data[index];
    }

    constexpr const T& operator[](u64 index) const
    {
        sky_assert(m_data && index < m_size);
        return m_data[index];
    }

    constexpr T& front()
    {
        sky_assert(m_data && m_size);
        return m_data[0];
    }

    [[nodiscard]] constexpr const T& front() const
    {
        sky_assert(m_data && m_size);
        return m_data[0];
    }

    constexpr T& back()
    {
        sky_assert(m_data && m_size);
        return m_data[m_size - 1];
    }

    [[nodiscard]] constexpr const T& back() const
    {
        sky_assert(m_data && m_size);
        return m_data[m_size - 1];
    }

    constexpr T* begin() { return std::addressof(m_data[0]); }

    [[nodiscard]] constexpr const T* begin() const { return std::addressof(m_data[0]); }

    constexpr T* end()
    {
        sky_assert(!(m_data == nullptr && m_size > 0));
        return std::addressof(m_data[m_size]);
    }

    [[nodiscard]] constexpr const T* end() const
    {
        sky_assert(!(m_data == nullptr && m_size > 0));
        return std::addressof(m_data[m_size]);
    }


private:
    constexpr void move(heap_array& o)
    {
        m_size = o.m_size;
        m_data = o.m_data;
        o.reset();
    }

    constexpr void reset()
    {
        m_size = 0;
        m_data = nullptr;
    }

    constexpr void destruct_range(u64 first, u64 last)
    {
        sky_assert(first <= m_size && last <= m_size && first <= last);

        if (!m_data)
            return;

        while (first != last)
        {
            m_data[first].~T();
            ++first;
        }
    }

    constexpr void destroy()
    {
        sky_assert([&] { return m_size ? m_data != nullptr : m_data == nullptr; }());
        destruct_range(0, m_size);
        free(m_data);
        m_size = 0;
        m_data = nullptr;
    }

    u64 m_size{};
    T*  m_data{};
};
} // namespace sky::utl