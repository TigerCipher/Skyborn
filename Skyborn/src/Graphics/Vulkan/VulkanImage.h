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
//  File Name: VulkanImage.h
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "VulkanCommon.h"

namespace sky::graphics::vk::image
{

void create(const vulkan_context* context, VkImageType image_type, u32 width, u32 height, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view_flag,
            VkImageAspectFlags view_aspect_flags, vulkan_image* out_image);

void create_view(const vulkan_context* context, VkFormat format, vulkan_image* image, VkImageAspectFlags aspect_flags);

void destroy(const vulkan_context* context, vulkan_image* image);

} // namespace sky::graphics::vk::image