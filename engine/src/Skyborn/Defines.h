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
// File Name: Defines.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <concepts>
#include <cstdint>
#include <memory>

// Primitive types

// Unsigned
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Signed
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;


// Floating point
using f32 = float;
using f64 = double;

// Invalid unsigned values (-1)
constexpr u8  u8_invalid{ 0xffui8 };
constexpr u16 u16_invalid{ 0xffffui16 };
constexpr u32 u32_invalid{ 0xffff'ffffui32 };
constexpr u64 u64_invalid{ 0xffff'ffff'ffff'ffffui64 };

// Same as above, but for situations where the value is actually valid
constexpr u8  u8_max{ 0xffui8 };
constexpr u16 u16_max{ 0xffffui16 };
constexpr u32 u32_max{ 0xffff'ffffui32 };
constexpr u64 u64_max{ 0xffff'ffff'ffff'ffffui64 };

#if defined(_WIN32)
    #define SKY_PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "Skyborn only supports x64"
    #endif
#endif
// TODO: When I decide to work on other platforms, more macros need to be added here


// DLL signature importing and exporting

#ifdef SKY_EXPORT
    #ifdef _MSC_VER
        #define SAPI __declspec(dllexport)
    #else
        #define SAPI __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define SAPI __declspec(dllimport)
    #else
        #define SAPI
    #endif
#endif

// Macro helpers / Misc

/**
 * @brief Extracts a range of bits from the given number
 * @param number The number to extract from
 * @param p The bit position to pull from, inclusive. Starts from least significant bit as position 1. See note.
 * @param n The number of bits to extract
 * 
 * @note Example usage:
 * @note u8 num = 46; // 0010 1110
 * @note u8 extracted = EXTRACT_BITS(num, 1, 4); // Extracts 1110 == 14
 * @note extracted = EXTRACT_BITS(num, 1, 8); // Extracts 0010 1110 == 46
 * @note extracted = EXTRACT_BITS(num, 5, 4); // Extracts 0000 0010 == 2
 */
#define EXTRACT_BITS(number, p, n) (((1 << (n)) - 1) & ((number) >> ((p) -1)))

/**
 * @brief Sets a range of bits of the given number
 * @param number The number to modify
 * @param p The bit position to set from, inclusive. Starts from least significant bit as position 1. See note.
 * @param n The number of bits to set
 * 
 * @note Example usage:
 * @note u8 num = 46; // 0010 1110
 * @note SET_BITS(num, 1, 4, 5); // num becomes 0010 0101 == 37
 * @note SET_BITS(num, 1, 8, 23); // num(37) becomes 0001 0111 == 23
 * @note SET_BITS(num, 5, 4, 7); // num(23) becomes 0111 0111 == 119
 */
#define SET_BITS(number, p, n, value)                                                                                  \
    {                                                                                                                  \
        typeof(number) mask = ((1 << (n)) - 1) << ((p) -1);                                                            \
        number &= ~mask;                                                                                               \
        number |= ((value) << ((p) -1)) & mask;                                                                        \
    }

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)
#define BIT(x)        (1 << (x))
#define SKY_DELETE(x)                                                                                                  \
    delete (x);                                                                                                        \
    (x) = nullptr;

#ifdef _DEBUG
    #define SDBG(x) x
#else
    #define SDBG(x)
#endif

#ifndef DISABLE_COPY
    #define DISABLE_COPY(T)                                                                                            \
        explicit T(const T&)   = delete;                                                                               \
        T& operator=(const T&) = delete
#endif

#ifndef DISABLE_MOVE
    #define DISABLE_MOVE(T)                                                                                            \
        explicit T(T&&)   = delete;                                                                                    \
        T& operator=(T&&) = delete
#endif

#ifndef DISABLE_COPY_AND_MOVE
    #define DISABLE_COPY_AND_MOVE(T)                                                                                   \
        DISABLE_COPY(T);                                                                                               \
        DISABLE_MOVE(T)

#endif


// Commonly used templates and concepts

template<typename T, typename... Args>
concept constructible_from_args = std::constructible_from<T, Args...>;

template<typename T>
concept primitive_type = std::is_arithmetic_v<T>;

constexpr auto operator""_KB(const u64 x)
{
    return x * 1024u;
}

constexpr auto operator""_MB(const u64 x)
{
    // x * 1024 * 1024
    return x * 1048576u;
}

constexpr auto operator""_GB(const u64 x)
{
    // x * 1024 * 1024 * 1024
    return x * 1073741824u;
}

template<typename T>
using ref = std::shared_ptr<T>;

template<typename T>
using scope = std::unique_ptr<T>;

template<typename T, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr ref<T> create_ref(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr scope<T> create_scope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}