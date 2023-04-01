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
//  File Name: Application.cpp
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Application.h"
#include "Debug/Logger.h"
#include "Platform/Platform.h"
#include "Event.h"
#include "Input.h"

namespace sky::app
{

namespace
{
scope<game, memory_tag::game> game_instance{ nullptr };
bool                          running{ false };
bool                          suspended{ false };
} // anonymous namespace

bool create(scope<game, memory_tag::game> game)
{
    assert(!game_instance);
    if (game_instance)
    {
        LOG_ERROR("Application is already initialized");
        return false;
    }
    game_instance = std::move(game);
    const auto& [pos_x, pos_y, width, height, name]{ game_instance->desc() };

    running   = true;
    suspended = false;

    // Logger module
    if (!logger::initialize())
        return false;

    if (!input::initialize())
        return false;

    if (!events::initialize())
    {
        LOG_FATAL("Event system failed to initialize. Aborting...");
        return false;
    }

    if (!platform::initialize(name, pos_x, pos_y, width, height))
    {
        LOG_FATAL("Platform startup failed");
        return false;
    }

    LOG_INFO("Platform initialized");

    if (!game_instance->initialize())
    {
        LOG_FATAL("Game failed to initialize");
        return false;
    }

    LOG_INFO("Game initialized");

    //LOG_TRACE("Test message");
    //LOG_DEBUG("Test message");
    //LOG_INFO("Test message");
    //LOG_WARN("Test message");
    //LOG_ERROR("Test message");
    //LOG_FATAL("Test message");

    LOG_INFOF("Skyborn Version {} finished initializing", SKY_VERSION);

    return true;
}
bool run()
{
    while (running)
    {
        if (!platform::pump_messages())
            running = false;
        if (!suspended)
        {
            if (!game_instance->update(0.0f))
            {
                LOG_FATAL("Game tick failed, aborting");
                running = false;
                break;
            }

            if (!game_instance->render(0.0f))
            {
                LOG_FATAL("Game render failed, aborting");
                running = false;
                break;
            }

            input::update(0.0);
        }
    }

    events::shutdown();
    input::shutdown();
    platform::shutdown();
    logger::shutdown();
    return true;
}
} // namespace sky::app
