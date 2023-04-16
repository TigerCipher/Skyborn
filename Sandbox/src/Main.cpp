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
// File Name: Main.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include <Skyborn/Core/Platform.h>

#include <Skyborn/Debug/Logger.h>

using namespace sky;

int main(int argc, char** argv)
{
    LOG_TRACE("Test {} message", "trace");
    LOG_DEBUG("Test {} message", "debug");
    LOG_INFO("Test {} message", "info");
    LOG_WARN("Test {} message", "warn");
    LOG_ERROR("Test {} message", "error");
    LOG_FATAL("Test {} message", "fatal");
    platform::initialize("Sandbox", 300, 300, 1280, 720);

    while (true)
    {
        if (!platform::pump_messages())
        {
            break;
        }
    }

    platform::shutdown();

    return 0;
}