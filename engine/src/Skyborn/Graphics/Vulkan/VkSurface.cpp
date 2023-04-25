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
// File Name: VkSurface.cpp
// Date File Created: 04/22/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkSurface.h"
#include "VkCore.h"
#include "VkRenderpass.h"
#include "VkFramebuffer.h"
#include "Skyborn/Core/Application.h"
#include "Skyborn/Core/Platform.h"

#ifdef _WIN64
    #include <vulkan/vulkan_win32.h>
#endif


namespace sky::graphics::vk
{

namespace
{
struct framebuffer_cache
{
    u32 width{};
    u32 height{};
} fb_cache;

u32 framebuffer_size_generation{};

} // anonymous namespace

bool vk_surface::create()
{
    app::get_framebuffer_size(&fb_cache.width, &fb_cache.height);
    m_framebuffer_width  = fb_cache.width ? fb_cache.width : 1280;
    m_framebuffer_height = fb_cache.height ? fb_cache.height : 720;

    LOG_INFO("Creating Vulkan surface");
    if (!create_surface())
    {
        return false;
    }
    LOG_INFO("Creating Vulkan device");
    if (!core::create_device(m_surface))
    {
        return false;
    }
    LOG_INFO("Creating swapchain");
    m_swapchain.create(m_framebuffer_width, m_framebuffer_height, m_surface);
    m_renderpass = renderpass::create(m_swapchain.image_format(), core::device().depth_format,
                                      { 0.0f, 0.0f, (f32) m_framebuffer_width, (f32) m_framebuffer_height },
                                      { 0, 0, 0, 1 }, 1.0f, 0.0f);
    recreate_framebuffers();
    return true;
}

void vk_surface::destroy()
{
    for (auto& i : m_swapchain.framebuffers())
    {
        framebuffer::destroy(i);
    }

    LOG_INFO("Destroying renderpass");
    renderpass::destroy(m_renderpass);

    LOG_INFO("Destroying swapchain");
    m_swapchain.destroy();
    if (m_surface)
    {
        LOG_INFO("Destroying surface");
        vkDestroySurfaceKHR(core::instance(), m_surface, core::allocator());
        m_surface = nullptr;
    }
}

void vk_surface::present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore)
{
    sky_assert(m_surface);
    m_swapchain.present(graphics_queue, present_queue, render_complete_semaphore, m_image_index, m_framebuffer_width,
                        m_framebuffer_height, m_frame_index);
}

void vk_surface::on_resized(u32 width, u32 height)
{
    fb_cache.width  = width;
    fb_cache.height = height;
    ++framebuffer_size_generation;

    LOG_TRACE("Vulkan resized; ({}, {}) .. Generation: {}", width, height, framebuffer_size_generation);
}

bool vk_surface::create_surface()
{
#ifdef _WIN64
    VkWin32SurfaceCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hinstance = GetModuleHandle(nullptr);
    create_info.hwnd      = platform::get_window_handle();

    if (vkCreateWin32SurfaceKHR(core::instance(), &create_info, core::allocator(), &m_surface) != VK_SUCCESS)
    {
        LOG_FATAL("Vulkan surface creation failed");
        return false;
    }
#endif
    return true;
}

void vk_surface::recreate_framebuffers()
{
    utl::heap_array<VkImageView> attachments{ 2 };
    attachments[1] = m_swapchain.depth_attachment().image_view();
    for (u32 i = 0; i < m_swapchain.images().size(); ++i)
    {
        attachments[0] = m_swapchain.views()[i];
        m_swapchain.framebuffers()[i] =
            framebuffer::create(m_renderpass, m_framebuffer_width, m_framebuffer_height, attachments);
    }
}
} // namespace sky::graphics::vk