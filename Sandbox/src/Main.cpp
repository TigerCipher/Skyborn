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
#include "SandboxGame.h"
#include "Debug/Logger.h"
#include "Core/Application.h"

#pragma comment(lib, "Skyborn.lib")


using namespace sky;

int main(int argc, char* argv[])
{
    //platform::write_message("[WARNING] Just a test\n", 5);

    //logger::message(logger::log_level::debug, "Just a test with {} formatting. I have {} noses", "fancy", 3);
    LOG_TRACE("Test message");
    LOG_DEBUG("Test message");
    LOG_INFO("Test message");
    LOG_WARN("Test message");
    LOG_ERROR("Test message");
    LOG_FATAL("Test message");

    scope<sandbox_game> game{ create_scope<sandbox_game>(app::application_desc{ 100, 100, 1920, 1080, "Skyborn Sandbox" }) };

    if(!app::create(std::move(game)))
    {
        LOG_FATAL("Failed to create application");
        return 1;
    }

    if(!app::run())
    {
        LOG_FATAL("Application failed to shutdown properly");
        return 2;
    }

    return 0;
}
