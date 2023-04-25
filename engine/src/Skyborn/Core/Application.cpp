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
#include "Clock.h"
#include "Skyborn/Util/Util.h"
#include "Skyborn/Graphics/Renderer.h"

#include <filesystem>

// TODO: Move
#ifndef SKY_USE_SIMD
    #pragma message("NOTICE: To enable SIMD support, compile with SKY_USE_SIMD definition")
#else
    #pragma message("NOTICE: To disable SIMD support, remove the SKY_USE_SIMD definition from compile options")
#endif

namespace sky::app
{
namespace
{

ref<application_state> app_state;
const char*            current_working_directory;

// Event listeners
bool on_event(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, void* data)
{
    switch (code)
    {
    case events::system_event::application_quit:
        LOG_INFO("application_quit event received. Shutting down application");
        app_state->running = false;
        return true;
    default: return false;
    }
    return false;
}

bool on_key(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, void* data)
{
    const u16 key_code = KEY_CODE(data);
    if (code == events::system_event::key_pressed)
    {
        if (key_code == input::key::escape)
        {
            events::fire(events::system_event::application_quit, nullptr, nullptr);

            return true;
        }


        LOG_TRACE("{} ({}) key was pressed in window", (char) key_code, key_code);
    } else if (code == events::system_event::key_released)
    {
        LOG_TRACE("{} ({}) key was released in window", (char) key_code, key_code);
    }

    return false;
}

bool on_resized(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, void* data)
{
    const u16 width  = WINDOW_WIDTH(data);
    const u16 height = WINDOW_HEIGHT(data);

    if(width != app_state->width || height != app_state->height)
    {
        app_state->game_inst->app_desc.width = width;
        app_state->game_inst->app_desc.height = height;
        app_state->width = width;
        app_state->height = height;

        if(width == 0 || height == 0)
        {
            LOG_INFO("Window minimized. Suspending system");
            app_state->suspended = true;
            return true;
        }

        if(app_state->suspended)
        {
            LOG_INFO("Window restored. Resuming system");
            app_state->suspended = false;
        }

        app_state->game_inst->on_resize(app_state->game_inst, width, height);
        graphics::on_resized(width, height);
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
    // app_state            = ref<application_state>(game_inst->app_state.get());
    // app_state            = create_ref<application_state>();
    app_state            = game_inst->app_state;
    app_state->game_inst = game_inst;
    app_state->running   = false;
    app_state->suspended = false;

    if (!input::initialize())
    {
        LOG_FATAL("Input system failed to initialize");
        return false;
    }

    if (!events::initialize())
    {
        LOG_FATAL("Event system failed to initialize");
        return false;
    }

    events::register_event(events::system_event::application_quit, nullptr, on_event);
    events::register_event(events::system_event::key_pressed, nullptr, on_key);
    events::register_event(events::system_event::key_released, nullptr, on_key);
    events::register_event(events::system_event::resized, nullptr, on_resized);

    auto& [pos_x, pos_y, width, height, name] = game_inst->app_desc;

    if (!platform::initialize(name, pos_x, pos_y, width, height))
    {
        LOG_FATAL("Platform startup failed");
        return false;
    }

    if (!graphics::initialize(graphics::backend_api::vulkan, game_inst->app_desc.name))
    {
        LOG_FATAL("Failed to initialize renderer. Aborting...");
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
    core::clock clock{};
    core::clock fps_clock{};
    fps_clock.start();
    clock.start();
    clock.update();
    f64 last_time = clock.elapsed();
    u16 fps{};

    constexpr f64 target_frame_time = 1.0 / 60.0;

    while (app_state->running)
    {
        if (!platform::pump_messages())
        {
            app_state->running = false;
        }
        if (!app_state->suspended)
        {
            clock.update();
            const f64 current_time     = clock.elapsed();
            const f64 delta            = current_time - last_time;
            const f64 frame_start_time = platform::get_time();
            if (!app_state->game_inst->update(app_state->game_inst, (f32) delta))
            {
                LOG_FATAL("Game tick failed. Aborting...");
                app_state->running = false;
                break;
            }

            if (!app_state->game_inst->render(app_state->game_inst, (f32) delta))
            {
                LOG_FATAL("Game render failed. Aborting...");
                app_state->running = false;
                break;
            }

            graphics::render_packet packet{ (f32) delta };
            graphics::draw_frame(packet);
            ++fps;

            const f64 frame_end_time     = platform::get_time();
            const f64 frame_elapsed_time = frame_end_time - frame_start_time;

            if (const f64 remaining_seconds = target_frame_time - frame_elapsed_time; remaining_seconds > 0.0)
            {
                const u64      remaining_ms = (u64) (remaining_seconds * 1000.0);
                constexpr bool limit_frames = false; // TODO
                if (remaining_ms > 0 && limit_frames)
                {
                    utl::sleep((u32) remaining_ms);
                }
            }

            input::update(delta);

            fps_clock.update();
            if (fps_clock.elapsed() >= 1.0)
            {
                LOG_DEBUG("FPS: {}", fps);
                fps = 0;
                fps_clock.start();
            }

            last_time = current_time;
        }
    }

    LOG_INFO("Shutting down...");

    events::unregister_event(events::system_event::application_quit, nullptr, on_event);
    events::unregister_event(events::system_event::key_pressed, nullptr, on_key);
    events::unregister_event(events::system_event::key_released, nullptr, on_key);
    events::unregister_event(events::system_event::resized, nullptr, on_resized);

    events::shutdown();
    input::shutdown();
    graphics::shutdown();
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