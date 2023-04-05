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
// Date File Created: 03/30/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>

#include "Common.h"

namespace sky::app
{

struct application_desc
{
    i16 pos_x{};
    i16 pos_y{};
    i16 width{};
    i16 height{};

    std::string name{};
};


class game
{
public:
    explicit game(application_desc desc) : m_desc{ std::move(desc) }{}
    virtual ~game() = default;

    virtual bool initialize() = 0;
    virtual bool update(f32 delta) = 0;
    virtual bool render(f32 delta) = 0;

    virtual void on_resize(i32 width, i32 height) = 0;

    [[nodiscard]] constexpr const application_desc& desc() const { return m_desc; }

private:
    application_desc m_desc{};
};

SAPI bool create(scope<game, memory_tag::game> game);
SAPI bool run();

void get_framebuffer_size(u32* width, u32* height);

}