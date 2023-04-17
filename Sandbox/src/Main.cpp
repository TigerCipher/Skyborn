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
// File Name: Main.cpp
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include <Skyborn/Entrypoint.h>

#include "Game.h"

using namespace sky;

bool create_game(app::game* game_inst)
{
    constexpr app::application_desc desc{ 100, 100, 1280, 720, "Skyborn Sandbox" };
    game_inst->app_desc   = desc;
    game_inst->initialize = sandbox::init;
    game_inst->update     = sandbox::update;
    game_inst->render     = sandbox::render;
    game_inst->on_resize  = sandbox::on_resize;

    game_inst->state = nullptr;

    game_inst->app_state = nullptr;

    return true;
}

void shutdown_game(app::game* game_inst)
{
    game_inst->state = nullptr;
}