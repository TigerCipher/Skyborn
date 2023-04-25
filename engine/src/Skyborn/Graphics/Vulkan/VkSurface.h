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
// File Name: VkSurface.h
// Date File Created: 04/22/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "VkCommon.h"
#include "VkSwapchain.h"

namespace sky::graphics::vk
{
class vk_surface
{
public:
    constexpr vk_surface() = default;

    ~vk_surface()
    {
        if (m_surface)
        {
            destroy();
        }
    }

    bool create();
    void destroy();

    void present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore);
    void on_resized(u32 width, u32 height);

    constexpr VkSurfaceKHR handle() const { return m_surface; }

    const vk_swapchain& swapchain() const { return m_swapchain; }

    constexpr const vk_renderpass& renderpass() const { return m_renderpass; }

private:
    bool create_surface();
    void recreate_framebuffers();

    VkSurfaceKHR  m_surface{};
    vk_swapchain  m_swapchain{};
    vk_renderpass m_renderpass{};
    u32           m_image_index{};
    u32           m_frame_index{};
    bool          m_recreating{};
    u32           m_framebuffer_width{};
    u32           m_framebuffer_height{};
};

} // namespace sky::graphics::vk