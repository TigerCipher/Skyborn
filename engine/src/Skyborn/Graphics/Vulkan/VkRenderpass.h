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
// File Name: VkRenderpass.h
// Date File Created: 04/23/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "VkCommon.h"

namespace sky::graphics::vk::renderpass
{
vk_renderpass create(VkSurfaceFormatKHR swapchain_image_format, VkFormat depth_format, vec4 render_area,
                     vec4 clear_color, f32 depth, u32 stencil);
void          destroy(vk_renderpass& renderpass);
void          begin(const vk_renderpass& renderpass, vk_command_buffer& cmd_buffer, VkFramebuffer framebuffer);
void          end(vk_renderpass& renderpass, vk_command_buffer& cmd_buffer);
} // namespace sky::graphics::vk::renderpass
