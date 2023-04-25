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
// File Name: VkImage.cpp
// Date File Created: 04/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkImage.h"

#include "VkCore.h"

namespace sky::graphics::vk
{
void vk_image::create(VkImageType type, u32 width, u32 height, VkFormat format, VkImageTiling tiling,
                      VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view_flag,
                      VkImageAspectFlags aspect_flags)
{
    m_width  = width;
    m_height = height;
    VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    create_info.imageType     = type; //VK_IMAGE_TYPE_2D;
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

    const VkDevice logical_device = core::logical_device();

    VK_CALL(vkCreateImage(logical_device, &create_info, core::allocator(), &m_handle));

    // Check memory requirements
    VkMemoryRequirements memory_requirements{};
    vkGetImageMemoryRequirements(logical_device, m_handle, &memory_requirements);
    const i32 memory_type = core::find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == u32_invalid)
    {
        LOG_ERROR("Required memory type not found. Image is not valid");
    }

    // Allocate memory
    VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocate_info.allocationSize  = memory_requirements.size;
    allocate_info.memoryTypeIndex = memory_type;
    VK_CALL(vkAllocateMemory(logical_device, &allocate_info, core::allocator(), &m_memory));

    VK_CALL(vkBindImageMemory(logical_device, m_handle, m_memory, 0));

    if (create_view_flag)
    {
        m_image_view = nullptr;
        create_view(format, aspect_flags);
    }
}

void vk_image::create_view(VkFormat format, VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    create_info.image                       = m_handle;
    create_info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format                      = format;
    create_info.subresourceRange.aspectMask = aspect_flags;

    // TODO: Make configurable
    create_info.subresourceRange.baseMipLevel   = 0;
    create_info.subresourceRange.levelCount     = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount     = 1;

    VK_CALL(vkCreateImageView(core::logical_device(), &create_info, core::allocator(), &m_image_view));
}

void vk_image::destroy()
{
    if (m_image_view)
    {
        vkDestroyImageView(core::logical_device(), m_image_view, core::allocator());
        m_image_view = nullptr;
    }

    if (m_memory)
    {
        vkFreeMemory(core::logical_device(), m_memory, core::allocator());
        m_memory = nullptr;
    }

    if (m_handle)
    {
        vkDestroyImage(core::logical_device(), m_handle, core::allocator());
        m_handle = nullptr;
    }
}
} // namespace sky::graphics::vk