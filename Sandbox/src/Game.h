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
// File Name: Game.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include <Skyborn/Defines.h>
#include <Skyborn/Core/Application.h>

namespace sandbox
{
struct game_state
{
    f32 delta;
};

bool init(sky::app::game* game_inst);
bool update(sky::app::game* game_inst, f32 delta);
bool render(sky::app::game* game_inst, f32 delta);
void on_resize(sky::app::game* game_inst, u32 width, u32 height);
} // namespace sandbox