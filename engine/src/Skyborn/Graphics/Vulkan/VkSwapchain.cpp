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
// File Name: VkSwapchain.cpp
// Date File Created: 04/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkSwapchain.h"
#include "VkCore.h"
#include "Skyborn/Util/Maths.h"
#include "Skyborn/Core/Platform.h"

namespace sky::graphics::vk
{
void vk_swapchain::create(u32 width, u32 height, VkSurfaceKHR surface)
{
    m_surface = surface;
    create(width, height);
}

void vk_swapchain::create(u32 width, u32 height)
{
    sky_assert(m_surface);
    // core::query_swapchain_support(core::physical_device(), m_info);
    m_info = get_swapchain_support_info(core::physical_device(), m_surface);
    VkExtent2D swapchain_extent{ width, height };
    m_max_frames_in_flight = 2;

    bool found = false;
    for (const auto& format : m_info.formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
        {
            m_image_format = format;
            found          = true;
            break;
        }
    }

    if (!found)
    {
        m_image_format = m_info.formats[0];
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : m_info.present_modes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    // core::query_swapchain_support(core::physical_device(), m_info);

    const auto& capabilities = m_info.capabilities;

    if (capabilities.currentExtent.width != u32_max)
    {
        swapchain_extent = capabilities.currentExtent;
    }

    const VkExtent2D min    = capabilities.minImageExtent;
    const VkExtent2D max    = capabilities.maxImageExtent;
    swapchain_extent.width  = math::clamp(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = math::clamp(swapchain_extent.height, min.height, max.height);

    u32 image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count < capabilities.maxImageCount)
    {
        image_count = capabilities.maxImageCount;
    }

    swapchain_info(image_count, present_mode, swapchain_extent);

    core::framebuffer().current_frame = 0; // TODO: Replace

    create_images();
    create_views();

    if (!core::detect_depth_format())
    {
        core::device().depth_format = VK_FORMAT_UNDEFINED;
        LOG_FATAL("Failed to find a supported format");
    }

    m_depth_attachment.create(VK_IMAGE_TYPE_2D, swapchain_extent.width, swapchain_extent.height,
                              core::device().depth_format, VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true,
                              VK_IMAGE_ASPECT_DEPTH_BIT);
    LOG_INFO("Swapchain created");
}

void vk_swapchain::recreate(u32 width, u32 height)
{
    destroy();
    create(width, height);
}

void vk_swapchain::destroy()
{
    LOG_INFO("Destroying swapchain");
    m_depth_attachment.destroy();

    for (u32 i = 0; i < m_views.size(); ++i)
    {
        vkDestroyImageView(core::logical_device(), m_views[i], core::allocator());
    }

    vkDestroySwapchainKHR(core::logical_device(), m_handle, core::allocator());
    m_handle = nullptr;

    m_images.clear();
    m_views.clear();
}

bool vk_swapchain::acquire_next_image_index(u64 timeout, VkSemaphore image_available_semaphore, VkFence fence,
                                            u32 width, u32 height, u32& image_index)
{
    const VkResult result = vkAcquireNextImageKHR(core::logical_device(), m_handle, timeout, image_available_semaphore,
                                                  fence, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // Recreate swapchain and return out of render loop
        recreate(width, height);
        return false;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_FATAL("Failed to acquire swapchain image");
        return false;
    }
    return true;
}

void vk_swapchain::present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore,
                           u32 present_image_index)
{
    VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_complete_semaphore;
    present_info.swapchainCount     = 1;
    present_info.pImageIndices      = &present_image_index;
    present_info.pResults           = nullptr;

    const VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        // Recreate swapchain because it is either out of date, sub optimal, or a framebuffer resize occurred
        // recreate(core::framebuffer().width, core::framebuffer().height);
        recreate(platform::get_window_width(), platform::get_window_height());
    } else if (result != VK_SUCCESS)
    {
        LOG_FATAL("Failed to present swapchain image");
    }
}

void vk_swapchain::swapchain_info(u32 image_count, VkPresentModeKHR present_mode, VkExtent2D swapchain_extent)
{
    VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    create_info.surface          = core::surface();
    create_info.minImageCount    = image_count;
    create_info.imageFormat      = m_image_format.format;
    create_info.imageColorSpace  = m_image_format.colorSpace;
    create_info.imageExtent      = swapchain_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const u32 gfx_idx     = core::device().graphics_queue_index;
    const u32 present_idx = core::device().present_queue_index;

    if (gfx_idx != present_idx)
    {
        const u32 queue_family_indices[2]{ gfx_idx, present_idx };
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queue_family_indices;
    } else
    {
        create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices   = nullptr;
    }

    create_info.preTransform   = m_info.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = nullptr;

    VK_CALL(vkCreateSwapchainKHR(core::logical_device(), &create_info, core::allocator(), &m_handle));
}

void vk_swapchain::create_images()
{
    u32 image_count{};
    VK_CALL(vkGetSwapchainImagesKHR(core::logical_device(), m_handle, &image_count, nullptr));

    if (m_images.empty())
    {
        m_images.resize(image_count);
    }
    if (m_views.empty())
    {
        m_views.resize(image_count);
    }

    VK_CALL(vkGetSwapchainImagesKHR(core::logical_device(), m_handle, &image_count, m_images.data()));
}

void vk_swapchain::create_views()
{
    for (u32 i = 0; i < m_images.size(); ++i)
    {
        VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        view_info.image                           = m_images[i];
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = m_image_format.format;
        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;

        VK_CALL(vkCreateImageView(core::logical_device(), &view_info, core::allocator(), &m_views[i]));
    }
}

swapchain_support_info get_swapchain_support_info(VkPhysicalDevice pd, VkSurfaceKHR surface)
{
    swapchain_support_info swapchain{};
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, surface, &swapchain.capabilities));

    u32 format_count{};
    VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &format_count, nullptr));

    if (format_count)
    {
        if (swapchain.formats.empty())
        {
            swapchain.formats.resize(format_count);
        }

        VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &format_count, swapchain.formats.data()));
    }

    u32 present_modes_count{};
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &present_modes_count, nullptr));
    if (present_modes_count)
    {
        if (swapchain.present_modes.empty())
        {
            swapchain.present_modes.resize(present_modes_count);
        }
        VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &present_modes_count,
                                                          swapchain.present_modes.data()));
    }

    return swapchain;
}
} // namespace sky::graphics::vk