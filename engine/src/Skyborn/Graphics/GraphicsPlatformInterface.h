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
// File Name: GraphicsPlatformInterface.h
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"
#include "Renderer.h"

namespace sky::graphics
{

struct platform_interface
{
    bool (*initialize)(const char* app_name);
    void (*shutdown)(void);
    void (*resized)(u16 width, u16 height);
    bool (*begin_frame)(f32 delta);
    bool (*end_frame)(f32 delta);

    u64 frame_number{};

    backend_api platform = (backend_api) -1;
};
} // namespace sky::graphics