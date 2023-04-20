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
// File Name: Renderer.cpp
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "Renderer.h"

#include "GraphicsPlatformInterface.h"
#include "Vulkan/VkInterface.h"

#include "Skyborn/Debug/Logger.h"
#include "Skyborn/Debug/Asserts.h"

namespace sky::graphics
{
namespace
{
platform_interface gfx{};

bool set_platform_interface(backend_api api, platform_interface& plat_interface)
{
    switch (api)
    {
    case backend_api::vulkan: vk::get_platform_interface(plat_interface); break;
    default: return false;
    }

    sky_assert(plat_interface.platform == api);
    return true;
}

bool end_frame(f32 delta)
{
    const bool res = gfx.end_frame(delta);
    ++gfx.frame_number;
    return res;
}

} // anonymous namespace

bool initialize(backend_api api, const char* app_name)
{
    return set_platform_interface(api, gfx) && gfx.initialize(app_name);
}

void shutdown()
{
    if (gfx.platform != (backend_api) -1)
    {
        gfx.shutdown();
    }
}

void on_resized(u16 width, u16 height)
{
    gfx.resized(width, height);
}

bool draw_frame(const render_packet& packet)
{
    if (gfx.begin_frame(packet.delta))
    {
        if (!end_frame(packet.delta))
        {
            LOG_ERROR("Graphics backend end_frame failed. Aborting...");
            return false;
        }
    }
    return true;
}

} // namespace sky::graphics