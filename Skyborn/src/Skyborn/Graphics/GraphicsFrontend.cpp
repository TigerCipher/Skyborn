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
//  File Name: GraphicsFrontend.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "GraphicsFrontend.h"
#include "GraphicsBackend.h"

namespace sky::graphics
{

namespace
{
graphics_backend* backend{ nullptr };

bool end_frame(f32 delta)
{
    const bool res{ backend->end_frame(delta) };
    ++backend->frame_number;
    return res;
}

} // anonymous namespace

bool initialize(const char* app_name)
{
    backend = (graphics_backend*) memory::allocate(sizeof(graphics_backend), memory_tag::renderer);

    // TODO: Let API be chosen by user (potentially player?)
    create_backend(graphics_backend_api::vulkan, backend);

    if (!backend->initialize(app_name))
    {
        LOG_FATAL("Failed to initialize graphics backend. Aborting...");
        return false;
    }

    return true;
}

void shutdown()
{
    backend->shutdown();
    memory::free(backend, sizeof(graphics_backend), memory_tag::renderer);
}

void on_resized(u16 width, u16 height)
{
    if (backend)
    {
        backend->resized(width, height);
    } else
    {
        LOG_WARN("Vulkan backend does not exist");
    }
}

bool draw_frame(const render_packet* packet)
{
    if (backend->begin_frame(packet->delta))
    {
        if (!end_frame(packet->delta))
        {
            LOG_ERROR("graphics_backend::end_frame failed. Aborting...");
            return false;
        }
    }

    return true;
}


} // namespace sky::graphics