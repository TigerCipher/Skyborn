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
//  File Name: Logger.cpp
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Logger.h"

#include "Skyborn/Platform/Platform.h"

namespace sky::logger
{

bool initialize()
{
    // TODO: Create log file
    return true;
}

void shutdown()
{
    // TODO: Clear queued messages
}

namespace detail
{
void message(log_level::level lvl, std::string_view msg)
{
    const bool  is_error{ lvl > log_level::warn };
    std::string str;
    switch (lvl)
    {
    case log_level::trace: str = std::format("[ TRACE ]: {}\n", msg); break;
    case log_level::debug: str = std::format("[ DEBUG ]: {}\n", msg); break;
    case log_level::info: str = std::format("[ INFO ]: {}\n", msg); break;
    case log_level::warn: str = std::format("[ WARNING ]: {}\n", msg); break;
    case log_level::error: str = std::format("[ ERROR ]: {}\n", msg); break;
    case log_level::fatal: str = std::format("[ FATAL ]: {}\n", msg); break;
    }

    if (is_error)
    {
        platform::write_error(str, lvl);
    } else
    {
        platform::write_message(str, lvl);
    }
}
} // namespace detail

} // namespace sky::logger
