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
//  File Name: Util.h
//  Date File Created: 04/01/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"

#include <string_view>
#include <thread>

namespace sky::utl
{

// C-String utils. For certain circumstances I do find std::strings to have a bit more overhead than I'd like
// So in cases where I prefer to use c-strings, these utility functions should prove handy

inline u64 string_length(std::string_view str)
{
    return str.length();
}

// Will allocate memory
char* copy_string(const char* str);

// For comparing c-strings. I can't help but see `strcmp(str0, str1) == 0` and think it means they aren't equal, when they are
inline bool string_compare(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

inline void sleep(u32 milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{milliseconds});
}

void change_working_directory(std::string_view path);

}