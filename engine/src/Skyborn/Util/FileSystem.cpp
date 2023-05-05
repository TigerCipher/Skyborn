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
// File Name: FileSystem.cpp
// Date File Created: 05/04/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "FileSystem.h"

#include "Skyborn/Debug/Logger.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

namespace sky::utl::fs
{
bool exists(const char* path)
{
    struct stat buffer
    {};

    return stat(path, &buffer) == 0;
}

bool open(const char* path, u8 mode, bool binary, file_handle& handle)
{
    handle.handle   = nullptr;
    handle.is_valid = false;
    const char* mode_str;

    if ((mode & file_modes::read) != 0 && (mode & file_modes::write) != 0 && (mode & file_modes::append) == 0)
    {
        mode_str = binary ? "w+b" : "w+";
    } else if ((mode & file_modes::read) != 0 && (mode & file_modes::write) == 0 && (mode & file_modes::append) == 0)
    {
        mode_str = binary ? "rb" : "r";
    } else if ((mode & file_modes::read) == 0 && (mode & file_modes::write) != 0 && (mode & file_modes::append) == 0)
    {
        mode_str = binary ? "wb" : "w";
    } else if ((mode & file_modes::read) == 0 && (mode & file_modes::write) == 0 && (mode & file_modes::append) != 0)
    {
        mode_str = binary ? "ab" : "a";
    } else if ((mode & file_modes::read) != 0 && (mode & file_modes::write) == 0 && (mode & file_modes::append) != 0)
    {
        mode_str = binary ? "a+b" : "a+";
    } else
    {
        LOG_ERROR("Invalid file mode for '{}'", path);
        return false;
    }

    FILE* file = fopen(path, mode_str);
    if (!file)
    {
        LOG_ERROR("Error while opening file '{}'", path);
        return false;
    }

    handle.handle   = file;
    handle.is_valid = true;

    return true;
}

void close(file_handle& handle)
{
    if (handle.handle)
    {
        fclose((FILE*) handle.handle);
        handle.handle   = nullptr;
        handle.is_valid = false;
    }
}

bool seek(i32 position, file_handle& handle)
{
    return fseek((FILE*)handle.handle, 0, position);
}

bool read_line(const file_handle& handle, std::string& line)
{
    if (!handle.handle)
        return false;

    char buffer[max_line_length];
    if (fgets(buffer, max_line_length, (FILE*) handle.handle))
    {
        line = buffer;
        return true;
    }

    return false;
}

bool read_line_trim(const file_handle& handle, std::string& line)
{
    if (!read_line(handle, line))
        return false;

    std::erase(line, '\n');
    return true;
}

bool read(const file_handle& handle, u64 size, void* data, u64& read_size)
{
    if (!handle.handle)
        return false;

    read_size = fread(data, 1, size, (FILE*) handle.handle);
    if (read_size != size)
        return false;

    return true;
}

bool read_all_bytes(const file_handle& handle, vector<u8*>& bytes, u64& size_read)
{
    if (!handle.handle)
        return false;
    fseek((FILE*) handle.handle, 0, SEEK_END);
    const u64 size = ftell((FILE*) handle.handle);
    rewind((FILE*) handle.handle);
    bytes.resize(size);
    size_read = fread(bytes.data(), 1, size, (FILE*) handle.handle);

    if (size != size_read)
        return false;

    return true;
}

bool write_line(const file_handle& handle, const char* text)
{
    if (!handle.handle)
        return false;

    i32 result = fputs(text, (FILE*) handle.handle);
    if (result != EOF)
    {
        result = fputc('\n', (FILE*) handle.handle);
    }

    fflush((FILE*) handle.handle);
    return result != EOF;
}

bool write(const file_handle& handle, u64 size, const void* data, u64& written)
{
    if (!handle.handle)
        return false;
    written = fwrite(data, 1, size, (FILE*) handle.handle);
    if (written != size)
        return false;

    fflush((FILE*) handle.handle);
    return true;
}
} // namespace sky::utl::fs
