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
// File Name: VkSwapchain.h
// Date File Created: 04/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "VkCommon.h"
#include "VkImage.h"

namespace sky::graphics::vk
{

class vk_swapchain
{
public:
    constexpr vk_swapchain() = default;
    ~vk_swapchain()          = default;

    void create(u32 width, u32 height);
    void recreate(u32 width, u32 height);
    void destroy();
    bool acquire_next_image_index(u64 timeout, VkSemaphore image_available_semaphore, VkFence fence, u32& image_index);
    void present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore,
                 u32 present_image_index);

    constexpr VkSwapchainKHR handle() const { return m_handle; }

    constexpr VkSurfaceFormatKHR image_format() const { return m_image_format; }

    constexpr u8 max_frames_in_flight() const { return m_max_frames_in_flight; }

    constexpr utl::vector<VkImage> images() const { return m_images; }

    constexpr utl::vector<VkImageView> views() const { return m_views; }

    const vk_image& depth_attachment() const { return m_depth_attachment; }

private:
    void swapchain_info(u32 image_count, VkPresentModeKHR present_mode, VkExtent2D swapchain_extent);
    void create_images();
    void create_views();

    VkSwapchainKHR           m_handle{};
    VkSurfaceFormatKHR       m_image_format{};
    u8                       m_max_frames_in_flight{};
    utl::vector<VkImage>     m_images{};
    utl::vector<VkImageView> m_views{};
    vk_image                 m_depth_attachment{};
};
} // namespace sky::graphics::vk