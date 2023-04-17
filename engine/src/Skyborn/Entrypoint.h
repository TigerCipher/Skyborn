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
// File Name: Entrypoint.h
// Date File Created: 04/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"
#include "Skyborn/Debug/Logger.h"
#include "Skyborn/Core/Application.h"

extern bool create_game(sky::app::game* game_inst);
extern void shutdown_game(sky::app::game* game_inst);

// Entry point
int main(int argc, char** argv)
{
    sky::app::set_cwd(argv[0]);
    sky::app::game game_inst;

    if (!create_game(&game_inst))
    {
        LOG_FATAL("Failed to create game");
        return -1;
    }

    if (!sky::app::create(&game_inst))
    {
        LOG_FATAL("Failed to create application");
        return 1;
    }

    if (!sky::app::run())
    {
        LOG_FATAL("Application failed to shutdown properly");
        return 2;
    }

    shutdown_game(&game_inst);

    return 0;
}