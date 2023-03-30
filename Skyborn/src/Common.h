#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <cassert>

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

#define STRINGIFY(x) #x
#define BIT(x)       (1 << (x))
#define SKY_DELETE(x)                                                                                                            \
    delete (x);                                                                                                                  \
    (x) = nullptr;

#ifdef _DEBUG
    #define SDBG(x) x
#else
    #define SDBG(x)
#endif

#ifndef DISABLE_COPY
    #define DISABLE_COPY(T)                                                                                                      \
        explicit T(const T&)   = delete;                                                                                         \
        T& operator=(const T&) = delete
#endif

#ifndef DISABLE_MOVE
    #define DISABLE_MOVE(T)                                                                                                      \
        explicit T(T&&)   = delete;                                                                                              \
        T& operator=(T&&) = delete
#endif

#ifndef DISABLE_COPY_AND_MOVE
    #define DISABLE_COPY_AND_MOVE(T)                                                                                             \
        DISABLE_COPY(T);                                                                                                         \
        DISABLE_MOVE(T)

#endif


// Commonly used templates and concepts

template<typename T, typename... Args>
concept constructible_from_args = std::constructible_from<T, Args...>;

template<typename T>
using scope = std::unique_ptr<T>;

template<typename T>
using ref = std::shared_ptr<T>;

template<typename T, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr scope<T> create_scope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}


template<typename T, typename... Args>
    requires constructible_from_args<T, Args...>
constexpr ref<T> create_ref(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

inline auto operator""_KB(const size_t x)
{
    return x * 1024u;
}

inline auto operator""_MB(const size_t x)
{
    // x * 1024 * 1024
    return x * 1048576u;
}

inline auto operator""_GB(const size_t x)
{
    // x * 1024 * 1024 * 1024
    return x * 1073741824u;
}