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
//  File Name: VulkanRenderPass.h
//  Date File Created: 04/02/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once

#include "VulkanCommon.h"

namespace sky::graphics::vk::renderpass
{

void create(vulkan_context* context, vulkan_renderpass* prenderpass, f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a, f32 depth, f32 stencil);

void destroy(const vulkan_context& context, vulkan_renderpass* prenderpass);

void begin(vulkan_command_buffer* cmd_buffer, const vulkan_renderpass& renderpass, VkFramebuffer frame_buffer);

void end(vulkan_command_buffer* cmd_buffer, vulkan_renderpass* prenderpass);

}