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
//  File Name: Util.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Util.h"

namespace sky::utl
{
u64 string_length(const char* str)
{
    return strlen(str);
}

u64 string_length(std::string_view str)
{
    return str.length();
}

char* copy_string(const char* str)
{
    const u64  len{ string_length(str) };
    auto copy{ (char*) memory::allocate(len + 1, memory_tag::string) };
    memory::copy(copy, str, len + 1);
    return copy;
}

bool string_compare(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}
} // namespace sky::utl
