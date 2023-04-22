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
// File Name: VkImage.h
// Date File Created: 04/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "VkCommon.h"

namespace sky::graphics::vk
{
class vk_image
{
public:
    constexpr vk_image() = default;

    ~vk_image()
    {
        if (m_image_view || m_handle || m_memory)
        {
            destroy();
        }
    }

    void create(VkImageType type, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                VkMemoryPropertyFlags memory_flags, bool create_view_flag, VkImageAspectFlags aspect_flags);
    void create_view(VkFormat format, VkImageAspectFlags aspect_flags);

    void destroy();

    constexpr VkImage handle() const { return m_handle; }

    constexpr VkDeviceMemory memory() const { return m_memory; }

    constexpr VkImageView image_view() const { return m_image_view; }

    constexpr u32 width() const { return m_width; }

    constexpr u32 height() const { return m_height; }

private:
    VkImage        m_handle{};
    VkDeviceMemory m_memory{};
    VkImageView    m_image_view{};
    u32            m_width{};
    u32            m_height{};
};
} // namespace sky::graphics::vk