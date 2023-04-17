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
// File Name: PlatformWin32.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Platform.h"

#ifndef SKY_PLATFORM_WINDOWS
    #error This file should only be compiled on Windows
#endif

#include "Skyborn/Debug/Logger.h"
#include "Event.h"

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
    cyan,
    light_red,
    purple,
    yellow,
    white,
    red_bg_white_fg        = 79,
    light_blue_bg_black_fg = 144,
    light_blue_bg_white_fg = 159,
};

// trace, debug, info, warn, error, fatal
constexpr u8 levels[6]{ gray, light_gray, green, gold, light_red, purple };

struct platform_state
{
    HINSTANCE     hinst;
    HWND          hwnd;
    f64           clock_frequency;
    LARGE_INTEGER start_time;
} plat_state;

u16 original_console_state;
u16 original_console_error_state;

void setup_clock()
{
    LARGE_INTEGER frequency{};
    QueryPerformanceFrequency(&frequency);
    plat_state.clock_frequency = 1.0 / (f64) frequency.QuadPart;
    QueryPerformanceCounter(&plat_state.start_time);
}

LRESULT CALLBACK process_messages(HWND hwnd, u32 msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:
    {
        constexpr events::context ctx{};
        events::fire(events::system_event::application_quit, nullptr, ctx);
        return 0;
    }
    case WM_DESTROY:
        // LOG_TRACE("WM_DESTROY");
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
    {
        RECT r{};
        GetClientRect(hwnd, &r);
        u32 w{ (u32) r.right - r.left };
        u32 h{ (u32) r.bottom - r.top };

        events::context ctx{};
        ctx.data.u16[0] = (u16) w;
        ctx.data.u16[1] = (u16) h;
        events::fire(events::system_event::resized, nullptr, ctx);
    }
    break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        // const bool pressed{ msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN };
        // auto       k{ (input::key::code) wparam };
        // const bool is_extended{ (HIWORD(lparam) & KF_EXTENDED) == KF_EXTENDED };
        // if (wparam == VK_MENU)
        // {
        //     k = is_extended ? input::key::ralt : input::key::lalt;
        // } else if (wparam == VK_SHIFT)
        // {
        //     const u32 left_shift{ MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC) };
        //     const u32 scancode{ (u32) ((lparam & (0xff << 16)) >> 16) };
        //     k = scancode == left_shift ? input::key::lshift : input::key::rshift;
        // } else if (wparam == VK_CONTROL)
        // {
        //     k = is_extended ? input::key::rcontrol : input::key::lcontrol;
        // }

        // input::process_key(k, pressed);
    }
    break;
    case WM_MOUSEMOVE:
    {
        // const i32 x_pos{ GET_X_LPARAM(lparam) };
        // const i32 y_pos{ GET_Y_LPARAM(lparam) };
        // input::process_mouse_move((i16) x_pos, (i16) y_pos);
    }
    break;
    case WM_MOUSEWHEEL:
    {
        // if (i32 delta{ GET_WHEEL_DELTA_WPARAM(wparam) }; delta != 0)
        // {
        //     delta = delta < 0 ? -1 : 1;
        //     input::process_mouse_wheel((i8) delta);
        // }
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
        // const bool          pressed{ (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) };
        // input::button::code btn{ input::button::count };

        // switch (msg)
        // {
        // case WM_LBUTTONDOWN:
        // case WM_LBUTTONUP: btn = input::button::left; break;
        // case WM_RBUTTONDOWN:
        // case WM_RBUTTONUP: btn = input::button::right; break;
        // case WM_MBUTTONDOWN:
        // case WM_MBUTTONUP: btn = input::button::middle; break;
        // default: break;
        // }

        // if (btn != input::button::count)
        // {
        //     input::process_button(btn, pressed);
        // }
    }
    break;

    default: break;
    }

    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

bool create_window(const char* app_name, i32 x, i32 y, u32 width, u32 height)
{
    LOG_INFO("Creating window");
    plat_state.hinst = GetModuleHandleA(nullptr);
    const HICON icon = LoadIcon(plat_state.hinst, IDI_APPLICATION);
    WNDCLASSA   wc{};
    wc.style         = CS_DBLCLKS;
    wc.lpfnWndProc   = process_messages;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = plat_state.hinst;
    wc.hIcon         = icon;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // transparent bg
    wc.lpszClassName = "skyborn_window_class";

    if (!RegisterClassA(&wc))
    {
        LOG_FATAL("Window registration failed");
        return false;
    }

    const i32 clientx       = x;
    const i32 clienty       = y;
    const i32 client_width  = width;
    const i32 client_height = height;

    i32 winx       = clientx;
    i32 winy       = clienty;
    i32 win_width  = client_width;
    i32 win_height = client_height;

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
                                       win_height, nullptr, nullptr, plat_state.hinst, nullptr) };

    if (!handle)
    {
        LOG_FATAL("Window creation failed");
        return false;
    }
    plat_state.hwnd = handle;

    constexpr bool should_activate = true; // False for window with no input
    constexpr i32  show_win_command_flags{ should_activate ? SW_SHOW : SW_SHOWNOACTIVATE };
    ShowWindow(plat_state.hwnd, show_win_command_flags);

    setup_clock();
    LOG_INFO("Window created");

    return true;
}

} // anonymous namespace

bool initialize(const char* app_name, i32 x, i32 y, u32 width, u32 height)
{
    LOG_INFO("Booting up Skyborn platform");
    CONSOLE_SCREEN_BUFFER_INFO console_info{};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
    original_console_state = console_info.wAttributes;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &console_info);
    original_console_error_state = console_info.wAttributes;

    // for (u16 i = 1; i < 255; ++i) // up to 255
    // {
    //     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
    //     std::string   str{ std::format("[{}]: Is this color\n", i) };
    //     const LPDWORD written{ nullptr };
    //     WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), (DWORD) str.length(), written, nullptr);
    // }

    return create_window(app_name, x, y, width, height);
}

void shutdown()
{
    LOG_INFO("Platform shutting down");
    if (plat_state.hwnd)
    {
        DestroyWindow(plat_state.hwnd);
        plat_state.hwnd = nullptr;
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

void write_message(const char* msg, u8 color)
{
    const HANDLE console_handle{ GetStdHandle(STD_OUTPUT_HANDLE) };
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(msg);
    const u64     len     = strlen(msg);
    const LPDWORD written = nullptr;
    WriteConsoleA(console_handle, msg, (DWORD) len, written, nullptr);
}

void write_error(const char* msg, u8 color)
{
    const HANDLE console_handle{ GetStdHandle(STD_ERROR_HANDLE) };
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(msg);
    const u64     len     = strlen(msg);
    const LPDWORD written = nullptr;
    WriteConsoleA(console_handle, msg, (DWORD) len, written, nullptr);
}

void reset_console()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), original_console_state);
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), original_console_error_state);
}

f64 get_time()
{
    if (!plat_state.clock_frequency)
    {
        setup_clock();
    }
    LARGE_INTEGER now_time{};
    QueryPerformanceCounter(&now_time);
    return (f64) now_time.QuadPart * plat_state.clock_frequency;
}


} // namespace sky::platform