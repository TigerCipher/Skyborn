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
#include "Core/Input.h"

#if SKY_PLATFORM_WINDOWS

    #include <Windows.h>
    #include <windowsx.h>
    #include <cstdlib>

namespace sky::platform
{

namespace
{

enum colors : u8
{
    black = 0,
    dark_blue,
    dark_green,
    light_blue,
    red,
    dark_purple,
    gold,
    light_gray,
    gray,
    blue,
    green,
    light_red,
    purple,
    yellow,
    white,
    red_bg_white_fg        = 79,
    light_blue_bg_black_fg = 144,
    light_blue_bg_white_fg = 159,
};

// trace, debug, info, warn, error, fatal
constexpr u8 levels[6]{ gray, light_gray, green, gold, red, red_bg_white_fg };

u16 original_console_state;
u16 original_console_error_state;

HINSTANCE hinst;
HWND      hwnd;

LRESULT CALLBACK process_messages(HWND hwnd, u32 msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:
        // TODO Close event
        LOG_TRACE("WM_CLOSE");
        return 0;
    case WM_DESTROY:
        LOG_TRACE("WM_DESTROY");
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
    {
        //        RECT r;
        //        GetClientRect(hwnd, &r);
        //        s32 w = r.right - r.left;
        //        s32 h = r.bottom - r.top;

        // Resize event
    }
    break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        const bool pressed{ msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN };
        const auto k{ (input::key::code) wparam };
        input::process_key(k, pressed);
    }
    break;
    case WM_MOUSEMOVE:
    {
        const i32 x_pos{ GET_X_LPARAM(lparam) };
        const i32 y_pos{ GET_Y_LPARAM(lparam) };
        input::process_mouse_move((i16) x_pos, (i16) y_pos);
    }
    break;
    case WM_MOUSEWHEEL:
    {
        if (i32 delta{ GET_WHEEL_DELTA_WPARAM(wparam) }; delta != 0)
        {
            delta = delta < 0 ? -1 : 1;
            input::process_mouse_wheel((i8) delta);
        }
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
        bool                pressed = (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN);
        input::button::code btn{ input::button::count };

        switch (msg)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: btn = input::button::left; break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP: btn = input::button::right; break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: btn = input::button::middle; break;
        default: break;
        }

        if(btn != input::button::count)
        {
            input::process_button(btn, pressed);
        }
    }
    break;

    default: break;
    }

    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

bool create_window(const char* app_name, i32 x, i32 y, i32 width, i32 height)
{
    hinst = GetModuleHandleA(nullptr);
    const HICON icon{ LoadIcon(hinst, IDI_APPLICATION) };
    WNDCLASSA   wc{};
    wc.style         = CS_DBLCLKS;
    wc.lpfnWndProc   = process_messages;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hinst;
    wc.hIcon         = icon;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // transparent bg
    wc.lpszClassName = "skyborn_window_class";

    if (!RegisterClassA(&wc))
    {
        LOG_FATAL("Window registration failed");
        return false;
    }

    const i32 clientx{ x };
    const i32 clienty{ y };
    const i32 client_width{ width };
    const i32 client_height{ height };

    i32 winx{ clientx };
    i32 winy{ clienty };
    i32 win_width{ client_width };
    i32 win_height{ client_height };

    u32           win_style{ WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION };
    constexpr u32 win_ex_style{ WS_EX_APPWINDOW };

    win_style |= WS_MAXIMIZEBOX;
    win_style |= WS_MINIMIZEBOX;
    win_style |= WS_THICKFRAME;

    RECT border_rect{ 0, 0, 0, 0 };
    AdjustWindowRectEx(&border_rect, win_style, false, win_ex_style);

    winx += border_rect.left;
    winy += border_rect.top;
    win_width += border_rect.right - border_rect.left;
    win_height += border_rect.bottom - border_rect.top;

    const HWND handle{ CreateWindowExA(win_ex_style, "skyborn_window_class", app_name, win_style, winx, winy, win_width,
                                       win_height, nullptr, nullptr, hinst, nullptr) };

    if (!handle)
    {
        LOG_FATAL("Window creation failed");
        return false;
    }
    hwnd = handle;

    constexpr bool should_activate{ true }; // False for window with no input
    constexpr i32  show_win_command_flags{ should_activate ? SW_SHOW : SW_SHOWNOACTIVATE };
    ShowWindow(hwnd, show_win_command_flags);

    return true;
}

} // anonymous namespace

bool initialize(std::string_view app_name, i32 x, i32 y, i32 width, i32 height)
{
    // Store original console info
    const HANDLE               console_handle{ GetStdHandle(STD_OUTPUT_HANDLE) };
    CONSOLE_SCREEN_BUFFER_INFO console_info{};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
    original_console_state = console_info.wAttributes;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &console_info);
    original_console_error_state = console_info.wAttributes;

    //for (u16 i{ 1 }; i < 255; ++i)
    //{
    //    SetConsoleTextAttribute(console_handle, i);
    //    std::string str{ std::format("[{}]: Is this color\n", i) };
    //    const LPDWORD written{ nullptr };
    //    WriteConsoleA(console_handle, str.c_str(), (DWORD) str.length(), written, nullptr);
    //}

    LOG_INFO("Skyborn starting up");

    create_window(app_name.data(), x, y, width, height);


    return true;
}

void shutdown()
{
    LOG_INFO("Skyborn shutting down");

    if (hwnd)
    {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }

    reset_console();
}


bool pump_messages()
{
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
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

void* allocate(u64 size, bool aligned /*= false*/)
{
    return malloc(size);
}

void free(void* block, bool aligned /*= false*/)
{
    std::free(block);
}

} // namespace sky::platform


#endif