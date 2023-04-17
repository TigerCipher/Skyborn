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
// File Name: Game.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "Game.h"

#include <Skyborn/Debug/Logger.h>

using namespace sky::app;

namespace sandbox
{
bool init(game* game_inst)
{
    LOG_INFO("Sandbox game initialized");
    return true;
}

bool update(game* game_inst, f32 delta)
{
    return true;
}

bool render(game* game_inst, f32 delta)
{
    return true;
}

void on_resize(game* game_inst, u32 width, u32 height) {}
} // namespace sandbox