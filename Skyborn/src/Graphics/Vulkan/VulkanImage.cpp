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
//  File Name: VulkanImage.cpp
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanImage.h"

#include "VulkanDevice.h"

namespace sky::graphics::vk::image
{
void create(const vulkan_context* context, VkImageType image_type, u32 width, u32 height, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view_flag,
            VkImageAspectFlags view_aspect_flags, vulkan_image* out_image)
{
    out_image->width  = width;
    out_image->height = height;

    VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    create_info.imageType     = VK_IMAGE_TYPE_2D;
    create_info.extent.width  = width;
    create_info.extent.height = height;
    create_info.extent.depth  = 1; // TODO: Make configurable
    create_info.mipLevels     = 4; // Needs support
    create_info.arrayLayers   = 1;
    create_info.format        = format;
    create_info.tiling        = tiling;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage         = usage;
    create_info.samples       = VK_SAMPLE_COUNT_1_BIT;     // TODO: Make configurable
    create_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE; // TODO: Make configurable

    VK_CHECK(vkCreateImage(context->device.logical, &create_info, context->allocator, &out_image->handle));

    // Check memory requirements

    VkMemoryRequirements memory_requirements{};
    vkGetImageMemoryRequirements(context->device.logical, out_image->handle, &memory_requirements);

    const i32 memory_type{ context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags) };
    if (memory_type == -1)
    {
        LOG_ERROR("Required memory type not found. Image is not valid.");
    }

    // Allocate memory

    VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocate_info.allocationSize  = memory_requirements.size;
    allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(vkAllocateMemory(context->device.logical, &allocate_info, context->allocator, &out_image->memory));

    VK_CHECK(vkBindImageMemory(context->device.logical, out_image->handle, out_image->memory, 0));

    if (create_view_flag)
    {
        out_image->view = nullptr;
        create_view(context, format, out_image, view_aspect_flags);
    }
}

void create_view(const vulkan_context* context, VkFormat format, vulkan_image* image, VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    create_info.image                       = image->handle;
    create_info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format                      = format;
    create_info.subresourceRange.aspectMask = aspect_flags;

    // TODO: Make configurable
    create_info.subresourceRange.baseMipLevel   = 0;
    create_info.subresourceRange.levelCount     = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount     = 1;

    VK_CHECK(vkCreateImageView(context->device.logical, &create_info, context->allocator, &image->view));
}

void destroy(const vulkan_context* context, vulkan_image* image)
{
    if (image->view)
    {
        vkDestroyImageView(context->device.logical, image->view, context->allocator);
        image->view = nullptr;
    }

    if (image->memory)
    {
        vkFreeMemory(context->device.logical, image->memory, context->allocator);
        image->memory = nullptr;
    }

    if (image->handle)
    {
        vkDestroyImage(context->device.logical, image->handle, context->allocator);
        image->handle = nullptr;
    }
}

} // namespace sky::graphics::vk::image
