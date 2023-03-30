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
// File Name: PlatformWindows.cpp
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Platform.h"
#include "Debug/Logger.h"

#if SKY_PLATFORM_WINDOWS

    #include <Windows.h>
    #include <windowsx.h>
    #include <cstdlib>

namespace sky::platform
{

namespace
{

constexpr u8 levels[6]{ 8, 1, 2, 6, 4, 64 };

u16 original_console_state;
u16 original_console_error_state;

} // anonymous namespace

bool initialize()
{

    // Store original console info
    const HANDLE console_handle{ GetStdHandle(STD_OUTPUT_HANDLE) };
    CONSOLE_SCREEN_BUFFER_INFO console_info{};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
    original_console_state = console_info.wAttributes;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &console_info);
    original_console_error_state = console_info.wAttributes;

    // Logger module
    if(!logger::initialize()) return false;
    
    return true;
}

void shutdown()
{
    logger::shutdown();

    reset_console();
}

void write_message(std::string_view msg, u8 color)
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

void reset_console()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), original_console_state);
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), original_console_error_state);
}

} // namespace sky::platform


#endif