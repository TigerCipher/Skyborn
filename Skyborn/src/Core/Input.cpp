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
//  File Name: Input.cpp
//  Date File Created: 03/31/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Input.h"
#include "Input.h"
#include "Event.h"

namespace sky::input
{

namespace
{
constexpr u16 max_keys{ 256 };

bool is_initialized{ false };

struct keyboard
{
    bool keys[max_keys]{};
};

struct mouse
{
    i16  x{};
    i16  y{};
    bool buttons[button::count]{};
};

keyboard current_keyboard{};
keyboard previous_keyboard{};

mouse current_mouse{};
mouse previous_mouse{};

} // anonymous namespace


bool initialize()
{
    is_initialized = true;
    LOG_INFO("Input submodule initialized");

    return true;
}

void shutdown()
{
    is_initialized = false;
    LOG_INFO("Input submodule shutdown");
}

void update(f64 delta)
{
    if (!is_initialized)
        return;

    memory::copy(&previous_keyboard, &current_keyboard, sizeof(keyboard));
    memory::copy(&previous_mouse, &current_mouse, sizeof(mouse));
}

void process_key(key::code key, bool pressed)
{
    // No need to handle if the state hasn't changed
    if (current_keyboard.keys[key] == pressed)
        return;

    current_keyboard.keys[key] = pressed; // update internal state

    // Invoke event for processing
    events::context ctx{};
    ctx.data.u16[0] = key;
    events::fire(pressed ? events::detail::system_event::key_pressed : events::detail::system_event::key_released, nullptr, ctx);
}

void process_button(button::code btn, bool pressed)
{
    // No need to handle if the state hasn't changed
    if (current_mouse.buttons[btn] == pressed)
        return;

    current_mouse.buttons[btn] = pressed; // update internal state

    // Invoke event for processing
    events::context ctx{};
    ctx.data.u16[0] = btn;
    events::fire(pressed ? events::detail::system_event::button_pressed : events::detail::system_event::button_released, nullptr,
                 ctx);
}

void process_mouse_move(i16 x, i16 y)
{
    // No need to handle if the state hasn't changed
    if (current_mouse.x == x && current_mouse.y == y)
        return;

#if 0
    LOG_TRACEF("Mouse Pos: ({}, {})", x, y);
#endif

    // Update internal state
    current_mouse.x = x;
    current_mouse.y = y;

    // Invoke event for processing
    events::context ctx{};
    ctx.data.u16[0] = x;
    ctx.data.u16[1] = y;
    events::fire(events::detail::system_event::mouse_moved, nullptr, ctx);
}

void process_mouse_wheel(i8 delta)
{
    events::context ctx{};
    ctx.data.u8[0] = delta;
    events::fire(events::detail::system_event::mouse_wheel, nullptr, ctx);
}

bool key_down(key::code key)
{
    if (!is_initialized)
        return false;

    return current_keyboard.keys[key];
}

bool key_up(key::code key)
{
    if (!is_initialized)
        return true;

    return !current_keyboard.keys[key];
}

bool was_key_down(key::code key)
{
    if (!is_initialized)
        return false;

    return previous_keyboard.keys[key];
}

bool was_key_up(key::code key)
{
    if (!is_initialized)
        return true;

    return !previous_keyboard.keys[key];
}

bool button_down(button::code btn)
{
    if (!is_initialized)
        return false;

    return current_mouse.buttons[btn];
}

bool button_up(button::code btn)
{
    if (!is_initialized)
        return true;

    return !current_mouse.buttons[btn];
}

bool was_button_down(button::code btn)
{
    if (!is_initialized)
        return false;

    return previous_mouse.buttons[btn];
}

bool was_button_up(button::code btn)
{
    if (!is_initialized)
        return true;

    return !previous_mouse.buttons[btn];
}


void get_mouse_position(i32& x, i32& y)
{
    if (!is_initialized)
    {
        x = 0;
        y = 0;
        return;
    }

    x = current_mouse.x;
    y = current_mouse.y;
}

void get_previous_mouse_position(i32& x, i32& y)
{
    if (!is_initialized)
    {
        x = 0;
        y = 0;
        return;
    }

    x = previous_mouse.x;
    y = previous_mouse.y;
}


} // namespace sky::input