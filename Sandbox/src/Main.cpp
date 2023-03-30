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
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Common.h"
#include "Platform/Platform.h"
#include "Debug/Logger.h"

#pragma comment(lib, "Skyborn.lib")


using namespace sky;

int main(int argc, char* argv[])
{
    platform::initialize();
    //platform::write_message("[WARNING] Just a test\n", 5);

    //logger::message(logger::log_level::debug, "Just a test with {} formatting. I have {} noses", "fancy", 3);
    LOG_TRACE("Test message");
    LOG_DEBUG("Test message");
    LOG_INFO("Test message");
    LOG_WARN("Test message");
    LOG_ERROR("Test message");
    LOG_FATAL("Test message");

    platform::shutdown();

    return 0;
}
