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
// File Name: Array.h
// Date File Created: 03/31/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Common.h"

template<typename T>
concept vector_enabled = std::is_copy_constructible_v<T> && std::is_default_constructible_v<T>;

namespace sky::utl
{

// We want memory layout to be:
// u64 capacity = number of elements it *can* hold
// u64 length / size = number of elements it actually holds
// T* data
template<vector_enabled T, memory_tag::tag Tag = memory_tag::vector, bool Destruct = true>
class vector
{
public:
    vector() = default;

    constexpr explicit vector(u64 count) { resize(count); }

    constexpr explicit vector(u64 count, const T& value) { resize(count, value); }


    // copy ctor
    constexpr vector(const vector& o) { *this = o; }

    // move ctor
    constexpr vector(vector&& o) noexcept : m_capacity{o.m_capacity}, m_size{o.m_size}, m_data{o.m_data} { o.reset(); }

    ~vector() { destroy(); }

    constexpr vector& operator=(const vector& o)
    {
        assert(this != std::addressof(o));
        if (this != std::addressof(o))
        {
            clear();
            reserve(o.m_size);
            for (auto& item : o)
            {
                emplace_back(item);
            }
            assert(m_size == o.m_size);
        }

        return *this;
    }

    constexpr vector& operator=(vector&& o) noexcept
    {
        assert(this != std::addressof(o));
        if (this != std::addressof(o))
        {
            destroy();
            move(o);
        }

        return *this;
    }

    constexpr void push_back(const T& value) { emplace_back(value); }

    constexpr void push_back(T&& value) { emplace_back(std::move(value)); }


    // Inserts items to the end of the vector
    template<typename... Params>
    constexpr decltype(auto) emplace_back(Params&&... p)
    {
        if (m_size == m_capacity)
        {
            reserve((m_capacity + 1) * 3 >> 1); // increases capacity by 50%
        }
        assert(m_size < m_capacity);

        T* const item {new (std::addressof(m_data[m_size])) T(std::forward<Params>(p)...)};
        ++m_size;
        return *item;
    }

    constexpr void resize(u64 new_size)
    {
        //static_assert(std::is_default_constructible_v<T>, "Type must have a default constructor");

        if (new_size > m_size)
        {
            reserve(new_size);
            while (m_size < new_size)
            {
                emplace_back();
            }
        } else if (new_size < m_size)
        {
            if constexpr (Destruct)
            {
                destruct_range(new_size, m_size);
            }

            m_size = new_size;
        }

        assert(new_size == m_size);
    }

    constexpr void resize(u64 new_size, const T& value)
    {
        //static_assert(std::is_copy_constructible_v<T>, "Type must be copyable");

        if (new_size > m_size)
        {
            reserve(new_size);
            while (m_size < new_size)
            {
                emplace_back(value);
            }
        } else if (new_size < m_size)
        {
            if constexpr (Destruct)
            {
                destruct_range(new_size, m_size);
            }

            m_size = new_size;
        }

        assert(new_size == m_size);
    }

    constexpr void reserve(u64 new_capacity)
    {
        if (new_capacity <= m_capacity)
            return;

        void* temp{ memory::allocate(new_capacity * sizeof(T), Tag) };
        memory::copy(temp, m_data, m_size * sizeof(T));
        if(m_data)
            memory::free(m_data, m_capacity * sizeof(T), Tag);
        m_data     = (T*) temp;
        m_capacity = new_capacity;
        //void* new_buffer = realloc(m_data, new_capacity * sizeof(T));
        //assert(new_buffer);
        //if (new_buffer)
        //{
        //    m_data     = static_cast<T*>(new_buffer);
        //    m_capacity = new_capacity;
        //}
    }

    constexpr T* const erase(u64 index)
    {
        assert(m_data && index < m_size);
        return erase(std::addressof(m_data[index]));
    }

    constexpr T* const erase(T* const item)
    {
        assert(m_data && item >= std::addressof(m_data[0]) && item < std::addressof(m_data[m_size]));

        if constexpr (Destruct)
            item->~T();
        --m_size;
        if (item < std::addressof(m_data[m_size]))
        {
            memory::copy(item, item + 1, (std::addressof(m_data[m_size]) - item) * sizeof(T));
        }

        return item;
    }

    constexpr T* const erase_unordered(u64 index)
    {
        assert(m_data && index < m_size);
        return erase_unordered(std::addressof(m_data[index]));
    }

    constexpr T* const erase_unordered(T* const item)
    {
        assert(m_data && item >= std::addressof(m_data[0]) && item < std::addressof(m_data[m_size]));

        if constexpr (Destruct)
            item->~T();
        --m_size;
        if (item < std::addressof(m_data[m_size]))
        {
            memory::copy(item, std::addressof(m_data[m_size]), sizeof(T));
        }

        return item;
    }

    constexpr void clear()
    {
        if constexpr (Destruct)
        {
            destruct_range(0, m_size);
        }
        m_size = 0;
    }

    constexpr void swap(vector& o) noexcept
    {
        if (this == std::addressof(o))
            return;
        auto temp(std::move(o));
        o.move(*this);
        *this = std::move(temp);
    }

    constexpr T* data() { return m_data; }

    // The T* vector
    [[nodiscard]] constexpr T* const data() const { return m_data; }

    [[nodiscard]] constexpr bool empty() const { return m_size == 0; }

    // The number of elements currently being contained
    [[nodiscard]] constexpr u64 size() const { return m_size; }

    // The number of elements that can be contained
    [[nodiscard]] constexpr u64 capacity() const { return m_capacity; }

    constexpr T& operator[](u64 index)
    {
        assert(m_data && index < m_size);
        return m_data[index];
    }

    constexpr const T& operator[](u64 index) const
    {
        assert(m_data && index < m_size);
        return m_data[index];
    }

    // May retrieve past size() but within capacity()
    constexpr T& at(u64 index)
    {
        assert(m_data && index < m_capacity);
        return m_data[index];
    }

    // May retrieve past size() but within capacity()
    constexpr T& at(u64 index) const
    {
        assert(m_data && index < m_capacity);
        return m_data[index];
    }

    constexpr T& front()
    {
        assert(m_data && m_size);
        return m_data[0];
    }

    [[nodiscard]] constexpr const T& front() const
    {
        assert(m_data && m_size);
        return m_data[0];
    }

    constexpr T& back()
    {
        assert(m_data && m_size);
        return m_data[m_size - 1];
    }

    [[nodiscard]] constexpr const T& back() const
    {
        assert(m_data && m_size);
        return m_data[m_size - 1];
    }

    constexpr T* begin() { return std::addressof(m_data[0]); }

    [[nodiscard]] constexpr const T* begin() const { return std::addressof(m_data[0]); }

    constexpr T* end()
    {
        assert(!(m_data == nullptr && m_size > 0));
        return std::addressof(m_data[m_size]);
    }

    [[nodiscard]] constexpr const T* end() const
    {
        assert(!(m_data == nullptr && m_size > 0));
        return std::addressof(m_data[m_size]);
    }

private:
    constexpr void move(vector& o)
    {
        m_capacity = o.m_capacity;
        m_size     = o.m_size;
        m_data     = o.m_data;
        o.reset();
    }

    constexpr void reset()
    {
        m_capacity = 0;
        m_size     = 0;
        m_data     = nullptr;
    }

    constexpr void destruct_range(u64 first, u64 last)
    {
        assert(Destruct);
        assert(first <= m_size && last <= m_size && first <= last);

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
        assert([&] { return m_capacity ? m_data != nullptr : m_data == nullptr; }());
        clear();
        if (m_data)
            memory::free(m_data, m_capacity * sizeof(T), Tag);
        m_capacity = 0;
        m_data     = nullptr;
    }

    u64 m_capacity{};
    u64 m_size{};
    T*  m_data{};
};

} // namespace sky::utl