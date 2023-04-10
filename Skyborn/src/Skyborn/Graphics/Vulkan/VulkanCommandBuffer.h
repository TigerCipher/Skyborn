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
//  File Name: VulkanCommandBuffer.h
//  Date File Created: 04/03/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once

#include "VulkanCommon.h"

namespace sky::graphics::vk::commands
{

void allocate_buffer(const vulkan_context& context, VkCommandPool pool, bool is_primary, vulkan_command_buffer* pbuffer);

void free_buffer(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer);

void begin(vulkan_command_buffer* pbuffer, bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use);

void end(vulkan_command_buffer* pbuffer);

void update_submitted(vulkan_command_buffer* pbuffer);

void reset(vulkan_command_buffer* pbuffer);

void allocate_buffer_begin_single_use(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer);

void end_single_use(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer, VkQueue queue);

}