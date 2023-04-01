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
#include "Clock.h"
#include "Graphics/GraphicsFrontend.h"

namespace sky::app
{

namespace
{
scope<game, memory_tag::game> game_instance{ nullptr };
bool                          running{ false };
bool                          suspended{ false };
core::clock                   clock{};

bool on_event(u16 code, void* sender, void* listener, events::context ctx)
{
    switch (code)
    {
    case events::detail::system_event::application_quit:
        LOG_INFO("application_quit event received. Shutting down application");
        running = false;
        return true;
    default: return false;
    }
}

bool on_key(u16 code, void* sender, void* listener, events::context ctx)
{
    if (code == events::detail::system_event::key_pressed)
    {
        const u16 key_code{ ctx.data.u16[0] };
        if (key_code == input::key::escape)
        {
            events::context c{};
            events::fire(events::detail::system_event::application_quit, nullptr, c);

            return true;
        }
        LOG_TRACEF("{} key was pressed in window", (char) key_code);
    } else if (code == events::detail::system_event::key_released)
    {
        const u16 key_code{ ctx.data.u16[0] };
        LOG_TRACEF("{} key was released in window", (char) key_code);
    }

    return false;
}

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

    // Register events
    events::register_event(events::detail::system_event::application_quit, nullptr, on_event);
    events::register_event(events::detail::system_event::key_pressed, nullptr, on_key);
    events::register_event(events::detail::system_event::key_released, nullptr, on_key);

    if (!platform::initialize(name, pos_x, pos_y, width, height))
    {
        LOG_FATAL("Platform startup failed");
        return false;
    }

    LOG_INFO("Platform initialized");

    if (!graphics::initialize(game_instance->desc().name.c_str()))
    {
        LOG_FATAL("Failed to initialize graphics frontend. Aborting...");
        return false;
    }

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
    clock.start();
    clock.update();
    f64 last_time{ clock.elapsed() };

    f64           running_time{};
    constexpr f64 target_frame_seconds{ 1.0 / 60.0 };
    u8            frame_count{};


    LOG_DEBUG(memory::get_usage_str());
    while (running)
    {
        if (!platform::pump_messages())
            running = false;
        if (!suspended)
        {
            clock.update();
            const f64 current_time{ clock.elapsed() };
            const f64 delta{ current_time - last_time };
            const f64 frame_start_time{ platform::get_time() };

            if (!game_instance->update((f32) delta))
            {
                LOG_FATAL("Game tick failed, aborting");
                running = false;
                break;
            }

            if (!game_instance->render((f32) delta))
            {
                LOG_FATAL("Game render failed, aborting");
                running = false;
                break;
            }

            graphics::render_packet packet{(f32)delta};
            graphics::draw_frame(&packet);

            const f64 frame_end_time{ platform::get_time() };
            const f64 frame_elapsed_time{ frame_end_time - frame_start_time };
            running_time += frame_elapsed_time;

            if (const f64 remaining_seconds{ target_frame_seconds - frame_elapsed_time }; remaining_seconds > 0.0)
            {
                const u64 remaining_ms{ (u64) remaining_seconds * 1000 };

                constexpr bool limit_frames{ false }; // For now, it's just false
                if (remaining_ms > 0 && limit_frames)
                {
                    platform::sleep(remaining_ms - 1);
                }

                ++frame_count;
            }

            input::update(delta);


            last_time = current_time;
        }
    }

    // Unregister events
    events::unregister_event(events::detail::system_event::application_quit, nullptr, on_event);
    events::unregister_event(events::detail::system_event::key_pressed, nullptr, on_key);
    events::unregister_event(events::detail::system_event::key_released, nullptr, on_key);

    events::shutdown();
    input::shutdown();
    graphics::shutdown();
    platform::shutdown();
    logger::shutdown();
    return true;
}
} // namespace sky::app
