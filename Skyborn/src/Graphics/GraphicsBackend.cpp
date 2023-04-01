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
//  File Name: GraphicsBackend.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "GraphicsBackend.h"
#include "Vulkan/VulkanBackend.h"

namespace sky::graphics
{

bool create_backend(graphics_backend_api::type api, graphics_backend* out_backend)
{
    if (api == graphics_backend_api::vulkan)
    {
        out_backend->initialize  = vk::initialize;
        out_backend->shutdown    = vk::shutdown;
        out_backend->begin_frame = vk::begin_frame;
        out_backend->end_frame   = vk::end_frame;
        out_backend->resized     = vk::on_resized;
        return true;
    }

    return false;
}

void destroy_backend(graphics_backend* gfx_backend)
{
    gfx_backend->initialize  = nullptr;
    gfx_backend->shutdown    = nullptr;
    gfx_backend->begin_frame = nullptr;
    gfx_backend->end_frame   = nullptr;
    gfx_backend->resized     = nullptr;
}


} // namespace sky::graphics::backend