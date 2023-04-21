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
// File Name: Platform.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"

#ifdef _WIN64
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
#endif

namespace sky::platform
{
#ifdef _WIN64
using window_handle   = HWND;
using window_instance = HINSTANCE;
#endif

SAPI bool initialize(const char* app_name, i32 x, i32 y, u32 width, u32 height);
SAPI void shutdown();

SAPI bool pump_messages();

void write_message(const char* msg, u8 color);
void write_error(const char* msg, u8 color);
void reset_console();

SAPI f64 get_time();

window_handle   get_window_handle();
window_instance get_window_instance();

} // namespace sky::platform