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
// File Name: Logger.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"

#include <format>

namespace sky::logger
{
struct log_level
{
    enum level : u8
    {
        trace,
        debug,
        info,
        warn,
        error,
        fatal
    };
};

template<class... Args>
void send_message(log_level::level lvl, const std::_Fmt_string<Args...> fmt, Args&&... args)
{
    std::string str = std::vformat(fmt._Str, std::make_format_args(args...));
    _send_message(lvl, str.c_str());
}

SAPI void _send_message(log_level::level lvl, const char* msg);

} // namespace sky::logger

#ifdef _DEBUG
    #define LOG_TRACE(msg, ...) sky::logger::send_message(sky::logger::log_level::trace, msg, ##__VA_ARGS__)
    #define LOG_DEBUG(msg, ...) sky::logger::send_message(sky::logger::log_level::debug, msg, ##__VA_ARGS__)
#else
    #define LOG_TRACE(msg, ...)
    #define LOG_DEBUG(msg, ...)
#endif


#define LOG_INFO(msg, ...)  sky::logger::send_message(sky::logger::log_level::info, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...)  sky::logger::send_message(sky::logger::log_level::warn, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) sky::logger::send_message(sky::logger::log_level::error, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) sky::logger::send_message(sky::logger::log_level::fatal, msg, ##__VA_ARGS__)