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

    explicit constexpr vk_swapchain(VkSurfaceKHR surface) : m_surface{ surface } {}

    ~vk_swapchain()
    {
        if (m_handle)
        {
            destroy();
        }
    }

    void create(u32 width, u32 height, u32& frame_index, VkSurfaceKHR surface);
    void create(u32 width, u32 height, u32& frame_index);
    void recreate(u32 width, u32 height, u32& frame_index);
    void destroy();
    bool acquire_next_image_index(u64 timeout, VkSemaphore image_available_semaphore, VkFence fence, u32 width,
                                  u32 height, u32& image_index, u32& frame_index);
    void present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore,
                 u32 present_image_index, u32 width, u32 height, u32& frame_index);

    [[nodiscard]] constexpr VkSwapchainKHR handle() const { return m_handle; }

    [[nodiscard]] constexpr VkSurfaceFormatKHR image_format() const { return m_image_format; }

    [[nodiscard]] constexpr u8 max_frames_in_flight() const { return m_max_frames_in_flight; }

    [[nodiscard]] constexpr const utl::vector<VkImage>& images() const { return m_images; }

    [[nodiscard]] constexpr const utl::vector<VkImageView>& views() const { return m_views; }

    [[nodiscard]] const vk_image& depth_attachment() const { return m_depth_attachment; }

    [[nodiscard]] constexpr const utl::heap_array<vk_framebuffer>& framebuffers() const { return m_framebuffers; }

    [[nodiscard]] utl::heap_array<vk_framebuffer>& framebuffers() { return m_framebuffers; }

    constexpr void set_info(swapchain_support_info info) { m_info = info; }

private:
    void swapchain_info(u32 image_count, VkPresentModeKHR present_mode, VkExtent2D swapchain_extent);
    void create_images();
    void create_views();

    swapchain_support_info          m_info{};
    VkSwapchainKHR                  m_handle{};
    VkSurfaceKHR                    m_surface{};
    VkSurfaceFormatKHR              m_image_format{};
    u8                              m_max_frames_in_flight{ 2 };
    utl::vector<VkImage>            m_images{};
    utl::vector<VkImageView>        m_views{};
    vk_image                        m_depth_attachment{};
    utl::heap_array<vk_framebuffer> m_framebuffers{};
};

swapchain_support_info get_swapchain_support_info(VkPhysicalDevice pd, VkSurfaceKHR surface);
} // namespace sky::graphics::vk