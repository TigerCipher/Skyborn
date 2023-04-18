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
// File Name: Input.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Input.h"

#include "Event.h"
#include "Skyborn/Debug/Logger.h"

namespace sky::input
{
namespace
{
constexpr u16 max_keys       = 256;
bool          is_initialized = false;

struct keyboard
{
    bool keys[max_keys]{};
};

struct mouse
{
    i16  x{};
    i16  y{};
    bool buttons[button::max_buttons]{};
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

    previous_keyboard = current_keyboard;
    previous_mouse    = current_mouse;
    // memcpy(&previous_keyboard, &current_keyboard, sizeof(keyboard));
    // memcpy(&previous_mouse, &current_mouse, sizeof(keyboard));
}

void process_key(key::code key, bool pressed)
{
    // No need to handle if the state hasn't changed
    if (current_keyboard.keys[key] == pressed)
        return;

    current_keyboard.keys[key] = pressed; // update internal state

    // Invoke event for processing
    u16 code = (u16) key;
    events::fire(pressed ? events::system_event::key_pressed : events::system_event::key_released, nullptr, &code);
}

void process_button(button::code btn, bool pressed)
{
    // No need to handle if the state hasn't changed
    if (current_mouse.buttons[btn] == pressed)
        return;

    current_mouse.buttons[btn] = pressed; // update internal state

    // Invoke event for processing
    u16 code = (u16) btn;
    events::fire(pressed ? events::system_event::button_pressed : events::system_event::button_released, nullptr,
                 &code);
}

void process_mouse_move(i16 x, i16 y)
{
    // No need to handle if the state hasn't changed
    if (current_mouse.x == x && current_mouse.y == y)
        return;

#if 0
    LOG_TRACE("Mouse Pos: ({}, {})", x, y);
#endif

    // Update internal state
    current_mouse.x = x;
    current_mouse.y = y;

    // Invoke event for processing
    u32 data = 0;
    SET_BITS(data, 1, 16, x);
    SET_BITS(data, 17, 16, y);
    events::fire(events::system_event::mouse_moved, nullptr, &data);
}

void process_mouse_wheel(i8 delta)
{
    events::fire(events::system_event::mouse_wheel, nullptr, &delta);
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

void get_mouse_position(i32* x, i32* y)
{
    if (!is_initialized)
    {
        *x = 0;
        *y = 0;
        return;
    }

    *x = current_mouse.x;
    *y = current_mouse.y;
}

void get_previous_mouse_position(i32* x, i32* y)
{
    if (!is_initialized)
    {
        *x = 0;
        *y = 0;
        return;
    }

    *x = previous_mouse.x;
    *y = previous_mouse.y;
}

} // namespace sky::input