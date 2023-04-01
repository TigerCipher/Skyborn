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
//  File Name: Skyborn.h
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Defines.h"
#include "Debug/Logger.h"
#include "Core/Application.h"
#include "Memory/Memory.h"

extern scope<sky::app::game, sky::memory_tag::game> create_game();

// Entry point
int main()
{
    sky::memory::initialize();
    scope<sky::app::game, sky::memory_tag::game> game{ create_game() };
    if (!game)
    {
        LOG_FATAL("Failed to create game");
        return -1;
    }

    if (!sky::app::create(std::move(game)))
    {
        LOG_FATAL("Failed to create application");
        return 1;
    }

    if (!sky::app::run())
    {
        LOG_FATAL("Application failed to shutdown properly");
        return 2;
    }


    // Before shutting down memory subsystem, check for tagged memory leaks
    // Some smart pointers may still be allocated, but will be evident with deallocation messages
    // printed after this memory leak check
    LOG_DEBUG("Shutting down memory subsystem...");
    LOG_DEBUG(sky::memory::get_usage_str());
    sky::memory::shutdown();

    return 0;
}