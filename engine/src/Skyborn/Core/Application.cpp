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
#include "Event.h"
#include "Input.h"

#include <filesystem>

namespace sky::app
{
namespace
{

ref<application_state> app_state;
const char*            current_working_directory;

// Event listeners
bool on_event(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, events::context ctx)
{
    switch (code)
    {
    case events::system_event::application_quit:
        LOG_INFO("application_quit event received. Shutting down application");
        app_state->running = false;
        return true;
    default: return false;
    }
}

bool on_key(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, events::context ctx)
{
    if (code == events::system_event::key_pressed)
    {
        const u16 key_code{ ctx.data.u16[0] };
        if (key_code == input::key::escape)
        {
            const events::context c{};
            events::fire(events::system_event::application_quit, nullptr, c);

            return true;
        }


        LOG_TRACE("{} ({}) key was pressed in window", (char) key_code, key_code);
    } else if (code == events::system_event::key_released)
    {
        const u16 key_code = ctx.data.u16[0];
        LOG_TRACE("{} ({}) key was released in window", (char) key_code, key_code);
    }

    return false;
}

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

    if (!events::initialize())
    {
        LOG_FATAL("Event system failed to initialize");
        return false;
    }

    events::register_event(events::system_event::application_quit, nullptr, on_event);
    events::register_event(events::system_event::key_pressed, nullptr, on_key);
    events::register_event(events::system_event::key_released, nullptr, on_key);

    auto& [pos_x, pos_y, width, height, name] = game_inst->app_desc;

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

    events::unregister_event(events::system_event::application_quit, nullptr, on_event);
    events::unregister_event(events::system_event::key_pressed, nullptr, on_key);
    events::unregister_event(events::system_event::key_released, nullptr, on_key);

    events::shutdown();
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