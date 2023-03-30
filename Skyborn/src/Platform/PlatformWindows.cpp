
#include "Platform.h"

#if SKY_PLATFORM_WINDOWS

    #include <Windows.h>
    #include <windowsx.h>
    #include <cstdlib>

namespace sky::platform
{

namespace
{

constexpr u8 levels[6]{ 8, 1, 2, 6, 4, 64 };

} // anonymous namespace

void write_console(std::string_view msg, u8 color)
{
    assert(color < 6);
    const HANDLE console_handle{ GetStdHandle(STD_OUTPUT_HANDLE) };
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(msg.data());
    const u64     len{ msg.length() };
    const LPDWORD written{ nullptr };
    WriteConsoleA(console_handle, msg.data(), (DWORD) len, written, nullptr);
}

void write_error(std::string_view msg, u8 color)
{
    assert(color < 6);
    const HANDLE console_handle{ GetStdHandle(STD_ERROR_HANDLE) };
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(msg.data());
    const u64     len{ msg.length() };
    const LPDWORD written{ nullptr };
    WriteConsoleA(console_handle, msg.data(), (DWORD) len, written, nullptr);
}

} // namespace sky::platform


#endif