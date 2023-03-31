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
//  File Name: SandboxGame.h
//  Date File Created: 03/30/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Core/Application.h"
#include "Debug/Logger.h"

class sandbox_game : public sky::app::game
{
public:
    explicit sandbox_game(sky::app::application_desc desc) : game{ std::move(desc) } {}

    bool initialize() override
    {
        LOG_DEBUG("Sandbox game initialize");
        return true;
    }
    bool update(f32 delta) override
    {
        LOG_DEBUG("Sandbox game update");
        return true;
    }
    bool render(f32 delta) override
    {
        LOG_DEBUG("Sandbox game render");
        return true;
    }
    void on_resize(i32 width, i32 height) override { LOG_DEBUG("Sandbox game on_resize"); }
};
