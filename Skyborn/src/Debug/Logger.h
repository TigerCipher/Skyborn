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
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <format>

#include "Common.h"

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

bool initialize();
void shutdown();

namespace detail
{
SAPI void message(log_level::level lvl, std::string_view msg);
} // namespace detail

} // namespace sky::logger

#ifdef _DEBUG
    #define LOG_TRACE(msg) sky::logger::detail::message(sky::logger::log_level::trace, msg)
    #define LOG_DEBUG(msg) sky::logger::detail::message(sky::logger::log_level::debug, msg)
#else
    #define LOG_TRACE(msg)
    #define LOG_DEBUG(msg)
#endif


#define LOG_INFO(msg)  sky::logger::detail::message(sky::logger::log_level::info, msg)
#define LOG_WARN(msg)  sky::logger::detail::message(sky::logger::log_level::warn, msg)
#define LOG_ERROR(msg) sky::logger::detail::message(sky::logger::log_level::error, msg)
#define LOG_FATAL(msg) sky::logger::detail::message(sky::logger::log_level::fatal, msg)