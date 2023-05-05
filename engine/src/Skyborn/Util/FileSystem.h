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
// File Name: FileSystem.h
// Date File Created: 05/04/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "Skyborn/Defines.h"
#include "Vector.h"

#include <string>

#define FILE_BEGIN SEEK_SET
#define FILE_END SEEK_END

namespace sky::utl::fs
{
constexpr u64 max_line_length = 32000;

struct file_handle
{
    void* handle;
    bool  is_valid;
};

struct file_modes
{
    enum mode : u8
    {
        read   = 0x1,
        write  = 0x2,
        append = 0x4,
    };
};

SAPI bool exists(const char* path);

SAPI bool open(const char* path, u8 mode, bool binary, file_handle& handle);


SAPI void close(file_handle& handle);

SAPI bool seek(i32 position, file_handle& handle);

SAPI bool read_line(const file_handle& handle, std::string& line);

// reads a line but trims the new line character away
SAPI bool read_line_trim(const file_handle& handle, std::string& line);

SAPI bool read(const file_handle& handle, u64 size, void* data, u64& read_size);

SAPI bool read_all_bytes(const file_handle& handle, vector<u8*>& bytes, u64& size);

SAPI bool write_line(const file_handle& handle, const char* text);

SAPI bool write(const file_handle& handle, u64 size, const void* data, u64& written);

} // namespace sky::utl::fs
