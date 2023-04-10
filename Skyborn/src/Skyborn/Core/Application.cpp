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

#include "Skyborn/Debug/Logger.h"
#include "Skyborn/Platform/Platform.h"
#include "Event.h"
#include "Input.h"
#include "Clock.h"
#include "Skyborn/Graphics/GraphicsFrontend.h"


#ifdef _DEBUG
    #define VERSION_MAJOR 0
    #define VERSION_MINOR 0
    #define VERSION_PATCH 0
    #define VERSION_BUILD 110

    #define SKY_VERSION                                                                                                          \
        STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH) "." STRINGIFY(VERSION_BUILD)

    #define SKY_ENGINE_NAME "Skyborn [Version " SKY_VERSION "]"
#else
    #include "Skyborn/Version.h"
#endif

namespace sky::app
{

namespace
{
scope<game, memory_tag::game> game_instance{ nullptr };
bool                          running{ false };
bool                          suspended{ false };
core::clock                   clock{};

bool on_event(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, events::context ctx)
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

bool on_key(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, events::context ctx)
{
    if (code == events::detail::system_event::key_pressed)
    {
        const u16 key_code{ ctx.data.u16[0] };
        if (key_code == input::key::escape)
        {
            const events::context c{};
            events::fire(events::detail::system_event::application_quit, nullptr, c);

            return true;
        }


        LOG_TRACEF("{} ({}) key was pressed in window", (char) key_code, key_code);
    } else if (code == events::detail::system_event::key_released)
    {
        const u16 key_code{ ctx.data.u16[0] };
        LOG_TRACEF("{} ({}) key was released in window", (char) key_code, key_code);
    }

    return false;
}

bool on_resized(u16 code, [[maybe_unused]] void* sender, [[maybe_unused]] void* listener, events::context ctx)
{
    if (code != events::detail::system_event::resized)
        return false;

    u16 width{ ctx.data.u16[0] };
    u16 height{ ctx.data.u16[1] };

    if (width != game_instance->desc().width || height != game_instance->desc().height)
    {
        game_instance->desc().width  = (i16) width;
        game_instance->desc().height = (i16) height;

        LOG_DEBUGF("Window Resize ({}, {})", width, height);

        if (width == 0 || height == 0)
        {
            LOG_INFO("Window minimized. Suspending system");
            suspended = true;
            return true;
        }
        if (suspended)
        {
            LOG_INFO("Window restored. Resuming system");
            suspended = false;
        }
        game_instance->on_resize(width, height);
        graphics::on_resized(width, height);
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
    utl::change_working_directory("../bin");

    LOG_INFOF("{} starting up...", SKY_ENGINE_NAME);

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
    events::register_event(events::detail::system_event::resized, nullptr, on_resized);

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

    LOG_INFOF("{} finished initializing", SKY_ENGINE_NAME);

    return true;
}

bool run()
{
    core::clock fps_clock{};
    fps_clock.start();
    u16 fps{};

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
            //const core::clock::time_point frame_start_time{ core::clock::high_res_clock::now() };
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

            graphics::render_packet packet{ (f32) delta };
            graphics::draw_frame(&packet);
            ++fps;

            //const core::clock::time_point frame_end_time{ core::clock::high_res_clock::now() };
            const f64 frame_end_time{ platform::get_time() };
            const f64 frame_elapsed_time{ frame_end_time - frame_start_time };
            //const core::clock::duration frame_elapsed_time{ frame_end_time - frame_start_time };
            running_time += frame_elapsed_time /*.count()*/;

            if (const f64 remaining_seconds{ target_frame_seconds - frame_elapsed_time /*.count()*/ }; remaining_seconds > 0.0)
            {
                //LOG_TRACEF("Remaining seconds: {}", remaining_seconds);
                const u64 remaining_ms{ (u64) (remaining_seconds * 1000) };

                constexpr bool limit_frames{ false }; // For now, it's just false
                if (remaining_ms > 0 && limit_frames)
                {
                    //LOG_DEBUGF("Remaining ms: {}", remaining_ms);
                    utl::sleep((u32) remaining_ms - 1);
                }

                ++frame_count;
            }

            input::update(delta);

            // TODO: When limiting fps to 60, this reports as 64-65. Need to figure out if it's because of how I count the fps, or if my frame limiter isn't quite correct
            fps_clock.update();
            if (fps_clock.elapsed() >= 1.0)
            {
                LOG_DEBUGF("FPS: {}", fps);
                fps = 0;
                fps_clock.start();
            }
            last_time = current_time;
        }
    }

    LOG_INFOF("{} shutting down...", SKY_ENGINE_NAME);

    // Unregister events
    events::unregister_event(events::detail::system_event::application_quit, nullptr, on_event);
    events::unregister_event(events::detail::system_event::key_pressed, nullptr, on_key);
    events::unregister_event(events::detail::system_event::key_released, nullptr, on_key);
    events::unregister_event(events::detail::system_event::resized, nullptr, on_resized);

    events::shutdown();
    input::shutdown();
    graphics::shutdown();
    platform::shutdown();
    logger::shutdown();
    return true;
}

void get_framebuffer_size(u32* width, u32* height)
{
    *width  = game_instance->desc().width;
    *height = game_instance->desc().height;
}

} // namespace sky::app
