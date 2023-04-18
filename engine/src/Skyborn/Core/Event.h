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
// File Name: Event.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "Skyborn/Defines.h"

namespace sky::events
{
struct system_event
{
    enum code : u16
    {
        // Shuts down application on the next frame
        application_quit = 0x01,

        // Keyboard press event. u16 key_code { ctx.data.u16[0] };
        key_pressed,

        // Keyboard release event. u16 key_code { ctx.data.u16[0] };
        key_released,

        // Mouse button press event. u16 button { ctx.data.u16[0] };
        button_pressed,

        // Mouse button press event. u16 button { ctx.data.u16[0] };
        button_released,

        /* Mouse moved event.
         * Usage:
         * u16 x { ctx.data.u16[0] };
         * u16 y { ctx.data.u16[2] };
         */
        mouse_moved,

        /* Mouse wheel scroll event.
         * Usage:
         * u8 delta { ctx.data.u8[0] };
         */
        mouse_wheel,

        /* Resize/Resolution Changed event.
         * Usage:
         * u16 width { ctx.data.u16[0] };
         * u16 height { ctx.data.u16[1] };
         */
        resized,

        count,
        max_code = 0xff
    };
};

struct context
{
    union
    {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
};

using func_on_event = bool (*)(u16 code, void* sender, void* listener, context ctx);

bool initialize();
void shutdown();

/**
 * Registers an event/listener/callback combination. Duplicates will return false
 * @param code The event code to listen for
 * @param listener A pointer to the listener. Can be null
 * @param on_event The callback function that will be invoked with the event is fired
 * @return true if the event was registered, false otherwise
 */
SAPI bool register_event(u16 code, void* listener, func_on_event on_event);


/**
 * Unregisters an event/listener/callback combination. Duplicates will return false
 * @param code The event code to stop listening for
 * @param listener A pointer to the listener. Can be null
 * @param on_event The callback function to be unregistered
 * @return true if the event was unregistered, false otherwise
 */
SAPI bool unregister_event(u16 code, void* listener, func_on_event on_event);

/**
 * Fires an event to all listeners of the said event. If handled, it should not be passed to any further listeners
 * @param code The event code to fire
 * @param sender A pointer to the sender. Can be null
 * @param ctx The event context data
 * @return true if handled, false otherwise
 */
SAPI bool fire(u16 code, void* sender, context ctx);

} // namespace sky::events