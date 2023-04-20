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
// File Name: VkInterface.cpp
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "VkInterface.h"

#include "VkCore.h"

namespace sky::graphics::vk
{
void get_platform_interface(platform_interface& plat_interface)
{
    plat_interface.initialize  = core::initialize;
    plat_interface.shutdown    = core::shutdown;
    plat_interface.resized     = core::resized;
    plat_interface.begin_frame = core::begin_frame;
    plat_interface.end_frame   = core::end_frame;


    plat_interface.platform = backend_api::vulkan;
}
} // namespace sky::graphics::vk