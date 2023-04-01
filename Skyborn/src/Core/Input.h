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
//  File Name: Input.h
//  Date File Created: 03/31/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"

namespace sky::input
{

struct button
{
    enum code : u16
    {
        left,
        right,
        middle,

        count
    };
};

struct key
{
    enum code : u16
    {
        backspace    = 0x08,
        enter        = 0x0D,
        tab          = 0x09,
        shift        = 0x10,
        control      = 0x11,
        pause        = 0x13,
        capital      = 0x14,
        escape       = 0x1B,
        convert      = 0x1C,
        nonconvert   = 0x1D,
        accept       = 0x1E,
        modechange   = 0x1F,
        space        = 0x20,
        prior        = 0x21,
        next         = 0x22,
        end          = 0x23,
        home         = 0x24,
        left         = 0x25,
        up           = 0x26,
        right        = 0x27,
        down         = 0x28,
        select       = 0x29,
        print        = 0x2A,
        execute      = 0x2B,
        snapshot     = 0x2C,
        insert       = 0x2D,
        del          = 0x2E,
        help         = 0x2F,
        zero         = 0x30,
        one          = 0x31,
        two          = 0x32,
        three        = 0x33,
        four         = 0x34,
        five         = 0x35,
        six          = 0x36,
        seven        = 0x37,
        eight        = 0x38,
        nine         = 0x39,
        a            = 0x41,
        b            = 0x42,
        c            = 0x43,
        d            = 0x44,
        e            = 0x45,
        f            = 0x46,
        g            = 0x47,
        h            = 0x48,
        i            = 0x49,
        j            = 0x4A,
        k            = 0x4B,
        l            = 0x4C,
        m            = 0x4D,
        n            = 0x4E,
        o            = 0x4F,
        p            = 0x50,
        q            = 0x51,
        r            = 0x52,
        s            = 0x53,
        t            = 0x54,
        u            = 0x55,
        v            = 0x56,
        w            = 0x57,
        x            = 0x58,
        y            = 0x59,
        z            = 0x5A,
        lwin         = 0x5B,
        rwin         = 0x5C,
        apps         = 0x5D,
        sleep        = 0x5F,
        numpad0      = 0x60,
        numpad1      = 0x61,
        numpad2      = 0x62,
        numpad3      = 0x63,
        numpad4      = 0x64,
        numpad5      = 0x65,
        numpad6      = 0x66,
        numpad7      = 0x67,
        numpad8      = 0x68,
        numpad9      = 0x69,
        multiply     = 0x6A,
        add          = 0x6B,
        separator    = 0x6C,
        subtract     = 0x6D,
        decimal      = 0x6E,
        divide       = 0x6F,
        f1           = 0x70,
        f2           = 0x71,
        f3           = 0x72,
        f4           = 0x73,
        f5           = 0x74,
        f6           = 0x75,
        f7           = 0x76,
        f8           = 0x77,
        f9           = 0x78,
        f10          = 0x79,
        f11          = 0x7A,
        f12          = 0x7B,
        f13          = 0x7C,
        f14          = 0x7D,
        f15          = 0x7E,
        f16          = 0x7F,
        f17          = 0x80,
        f18          = 0x81,
        f19          = 0x82,
        f20          = 0x83,
        f21          = 0x84,
        f22          = 0x85,
        f23          = 0x86,
        f24          = 0x87,
        numlock      = 0x90,
        scroll       = 0x91,
        numpad_equal = 0x92,
        lshift       = 0xA0,
        rshift       = 0xA1,
        lcontrol     = 0xA2,
        rcontrol     = 0xA3,
        lmenu        = 0xA4,
        rmenu        = 0xA5,
        semicolon    = 0xBA,
        plus         = 0xBB,
        comma        = 0xBC,
        minus        = 0xBD,
        period       = 0xBE,
        slash        = 0xBF,
        grave        = 0xC0,

        count
    };
};

bool initialize();
void shutdown();

void update(f64 delta);

void process_key(key::code key, bool pressed);

void process_button(button::code btn, bool pressed);
void process_mouse_move(i16 x, i16 y);
void process_mouse_wheel(i8 delta);



SAPI bool key_down(key::code key);
SAPI bool key_up(key::code key);
SAPI bool was_key_down(key::code key);
SAPI bool was_key_up(key::code key);

SAPI bool button_down(button::code btn);
SAPI bool button_up(button::code btn);
SAPI bool was_button_down(button::code btn);
SAPI bool was_button_up(button::code btn);

SAPI void get_mouse_position(i32& x, i32& y);
SAPI void get_previous_mouse_position(i32& x, i32& y);


} // namespace sky::input