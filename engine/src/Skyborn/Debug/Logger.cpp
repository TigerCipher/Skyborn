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
// File Name: Logger.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "Logger.h"

#include "Skyborn/Core/Platform.h"

#include <iomanip>
#include <ctime>
#include <sstream>

namespace sky::logger
{
void _send_message(log_level::level lvl, const char* msg)
{
    bool               is_error = lvl > log_level::warn;
    std::string        str;
    auto               t  = std::time(nullptr);
    auto               tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    std::string time_str = oss.str();
    switch (lvl)
    {
    case log_level::trace: str = std::format("[{}][ TRACE ]: {}\n", time_str, msg); break;
    case log_level::debug: str = std::format("[{}][ DEBUG ]: {}\n", time_str, msg); break;
    case log_level::info: str = std::format("[{}][ INFO ]: {}\n", time_str, msg); break;
    case log_level::warn: str = std::format("[{}][ WARNING ]: {}\n", time_str, msg); break;
    case log_level::error: str = std::format("[{}][ ERROR ]: {}\n", time_str, msg); break;
    case log_level::fatal: str = std::format("[{}][ FATAL ]: {}\n", time_str, msg); break;
    }

    if (is_error)
    {
        platform::write_error(str.c_str(), lvl);
    } else
    {
        platform::write_message(str.c_str(), lvl);
    }
}
} // namespace sky::logger

void report_assertion_failure(const char* expression, const char* message, const char* file, u32 line)
{
    sky::logger::send_message(sky::logger::log_level::fatal, "Assertion Failed: {}, message: '{}' at {}:{}", expression,
                              message, file, line);
}