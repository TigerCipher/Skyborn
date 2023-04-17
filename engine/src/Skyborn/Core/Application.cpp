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
// File Name: Application.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "Application.h"

#include "Skyborn/Debug/Logger.h"
#include "Platform.h"

#include <filesystem>

namespace sky::app
{
namespace
{

ref<application_state> app_state;
const char*            current_working_directory;

} // anonymous namespace

bool create(game* game_inst)
{
    if (game_inst->app_state)
    {
        LOG_ERROR("Application is already initialized");
        return false;
    }

    std::filesystem::path cwd = std::filesystem::current_path();
    LOG_DEBUG("Current working directory is set to: {}", cwd.string());

    LOG_INFO("Starting up application");
    game_inst->app_state = create_ref<application_state>();
    app_state            = ref<application_state>(game_inst->app_state.get());
    app_state->game_inst = game_inst;
    app_state->running   = false;
    app_state->suspended = false;

    const auto& [pos_x, pos_y, width, height, name] = game_inst->app_desc;
    if (!platform::initialize(name, pos_x, pos_y, width, height))
    {
        LOG_FATAL("Platform startup failed");
        return false;
    }

    if (!game_inst->initialize(game_inst))
    {
        LOG_FATAL("Failed to initialize game");
        return false;
    }

    LOG_INFO("Application initialized");
    return true;
}

bool run()
{
    app_state->running = true;

    while (app_state->running)
    {
        if (!platform::pump_messages())
        {
            app_state->running = false;
        }
        if (!app_state->suspended)
        {
            if (!app_state->game_inst->update(app_state->game_inst, 0.f))
            {
                LOG_FATAL("Game tick failed. Aborting...");
                app_state->running = false;
                break;
            }

            if (!app_state->game_inst->render(app_state->game_inst, 0.f))
            {
                LOG_FATAL("Game render failed. Aborting...");
                app_state->running = false;
                break;
            }
        }
    }

    LOG_INFO("Shutting down...");
    platform::shutdown();
    return true;
}

void get_framebuffer_size(u32* width, u32* height)
{
    *width  = app_state->width;
    *height = app_state->height;
}

void set_cwd(const char* cwd)
{
    std::filesystem::path exePath = std::filesystem::path(cwd).parent_path();
    std::filesystem::current_path(exePath);
    current_working_directory = std::filesystem::current_path().string().c_str();
}

const char* cwd()
{
    return current_working_directory;
}

} // namespace sky::app