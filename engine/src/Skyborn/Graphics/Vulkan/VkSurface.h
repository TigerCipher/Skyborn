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

    constexpr VkSurfaceKHR handle() const { return m_surface; }

    const vk_swapchain& swapchain() const { return m_swapchain; }

private:
    bool create_surface();

    VkSurfaceKHR m_surface{};
    vk_swapchain m_swapchain{};
    u32          m_image_index{};
    u32          m_frame_index{};
    bool         m_recreating{};
};
} // namespace sky::graphics::vk