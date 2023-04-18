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
// File Name: Event.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "Event.h"
#include "Skyborn/Util/Vector.h"
#include "Skyborn/Debug/Logger.h"

namespace sky::events
{
namespace
{

constexpr const char* event_names[] = {
    "application_quit", "key_pressed", "key_released", "button_pressed",
    "button_released",  "mouse_moved", "mouse_wheel",  "resized",
};

static_assert(_countof(event_names) == system_event::count - 1);

constexpr u32 max_message_codes = 2 << 13; // random magic number stuffs

bool is_initialized = false;

struct registered_event
{
    void*         listener;
    func_on_event callback;
};

struct event_code_entry
{
    utl::vector<registered_event> events{};
};

event_code_entry registered[max_message_codes];

} // anonymous namespace

bool initialize()
{
    if (is_initialized)
        return false;

    is_initialized = true;
    LOG_INFO("Events submodule initialized");
    return true;
}

void shutdown()
{
    for (u16 i = 0; i < max_message_codes; ++i)
    {
        if (!registered[i].events.empty())
        {
            registered[i].events.clear();
        }
    }
    LOG_INFO("Events submodule shutdown");
}

bool register_event(u16 code, void* listener, func_on_event on_event)
{
    if (!is_initialized)
        return false;

    const u64 register_count = registered[code].events.size();
    for (u64 i = 0; i < register_count; ++i)
    {
        if (registered[code].events[i].listener == listener)
        {
            // warn
            return false;
        }
    }

    const registered_event evt{ listener, on_event };
    registered[code].events.push_back(evt);
    LOG_TRACE("Registered event {}", event_names[code - 1]);
    return true;
}

bool unregister_event(u16 code, void* listener, func_on_event on_event)
{
    if (!is_initialized)
        return false;

    utl::vector<registered_event>& events = registered[code].events;
    for (u64 i = 0; i < events.size(); ++i)
    {
        if (auto [lsn, callback]{ events[i] }; lsn == listener && callback == on_event)
        {
            LOG_TRACE("Unregistered event {}", event_names[code - 1]);
            events.erase_unordered(i);
            return true;
        }
    }

    return false;
}

bool fire(u16 code, void* sender, context ctx)
{
    if (!is_initialized)
        return false;

    utl::vector<registered_event>& events = registered[code].events;
    for (u64 i = 0; i < events.size(); ++i)
    {
        if (auto [listener, callback]{ events[i] }; callback(code, sender, listener, ctx))
        {
            // If a listener "handled" an event, don't let remaining listeners handle as well
            return true;
        }
    }

    return false;
}

} // namespace sky::events