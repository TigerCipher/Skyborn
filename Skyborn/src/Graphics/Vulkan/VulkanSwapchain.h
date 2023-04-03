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
//  File Name: VulkanSwapchain.h
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "VulkanCommon.h"

namespace sky::graphics::vk::swapchain
{

void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_swapchain);

void recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_swapchain);

void destroy(const vulkan_context* context, vulkan_swapchain* swapchain);

bool acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout,
                              VkSemaphore image_available_semaphore, VkFence fence, u32* out_image_index);

void present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue,
             VkSemaphore render_complete_semaphore, u32 present_image_index);

} // namespace sky::graphics::vk::swapchain