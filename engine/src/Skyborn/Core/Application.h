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
// File Name: Application.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "Skyborn/Defines.h"
#include "Skyborn/Debug/Logger.h"

namespace sky::app
{
struct game;

struct application_state
{
    game* game_inst{ nullptr };
    bool  running{ false };
    bool  suspended{ false };
    u16   width{};
    u16   height{};
};

struct application_desc
{
    i16 pos_x{};
    i16 pos_y{};
    u16 width{};
    u16 height{};

    const char* name{};
};

struct game
{
    application_desc app_desc{};

    bool (*initialize)(game* game_inst);
    bool (*update)(game* game_inst, f32 delta);
    bool (*render)(game* game_inst, f32 delta);
    void (*on_resize)(game* game_inst, u32 width, u32 height);

    void* state;

    ref<application_state> app_state;
};

SAPI bool create(game* game_inst);
SAPI bool run();

SAPI void        set_cwd(const char* cwd);
SAPI const char* cwd();

void get_framebuffer_size(u32* width, u32* height);
} // namespace sky::app