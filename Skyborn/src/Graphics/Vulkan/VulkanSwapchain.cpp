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
//  File Name: VulkanSwapchain.cpp
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace sky::graphics::vk::swapchain
{
namespace
{

void swapchain_info(const vulkan_context& context, u32 image_count, vulkan_swapchain* out_swapchain,
                    VkPresentModeKHR present_mode, VkExtent2D swapchain_extent)
{
    VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    create_info.surface          = context.surface;
    create_info.minImageCount    = image_count;
    create_info.imageFormat      = out_swapchain->image_format.format;
    create_info.imageColorSpace  = out_swapchain->image_format.colorSpace;
    create_info.imageExtent      = swapchain_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Set up queue family indices
    const u32 gfx_idx{ context.device.graphics_queue_index };
    const u32 present_idx{ context.device.present_queue_index };
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
    create_info.preTransform   = context.device.swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = nullptr;

    VK_CHECK(vkCreateSwapchainKHR(context.device.logical, &create_info, context.allocator, &out_swapchain->handle));
}

void create_images(const vulkan_context& context, vulkan_swapchain* out_swapchain)
{
    out_swapchain->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context.device.logical, out_swapchain->handle, &out_swapchain->image_count, nullptr));
    out_swapchain->images.initialize(out_swapchain->image_count);
    out_swapchain->views.initialize(out_swapchain->image_count);
    //if (!out_swapchain->images)
    //{
    //    out_swapchain->images = (VkImage*) memory::allocate(sizeof(VkImage) * out_swapchain->image_count, memory_tag::renderer);
    //}

    //if (!out_swapchain->views)
    //{
    //    out_swapchain->views =
    //        (VkImageView*) memory::allocate(sizeof(VkImageView) * out_swapchain->image_count, memory_tag::renderer);
    //}

    VK_CHECK(vkGetSwapchainImagesKHR(context.device.logical, out_swapchain->handle, &out_swapchain->image_count,
                                     out_swapchain->images.data()));
}

void create_views(const vulkan_context& context, vulkan_swapchain* out_swapchain)
{
    for (u32 i = 0; i < out_swapchain->image_count; ++i)
    {
        VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        view_info.image                           = out_swapchain->images[i];
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = out_swapchain->image_format.format;
        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;

        VK_CHECK(vkCreateImageView(context.device.logical, &view_info, context.allocator, &out_swapchain->views[i]));
    }
}

} // anonymous namespace

void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_swapchain)
{
    VkExtent2D swapchain_extent{ width, height };
    out_swapchain->max_frames_in_flight = 2;

    // Choose a format
    bool found{ false };
    for (u32 i = 0; i < context->device.swapchain_support.format_count; ++i)
    {
        const VkSurfaceFormatKHR format{ context->device.swapchain_support.formats[i] };
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
        {
            out_swapchain->image_format = format;
            found                       = true;
            break;
        }
    }

    if (!found)
    {
        out_swapchain->image_format = context->device.swapchain_support.formats[0];
    }

    VkPresentModeKHR present_mode{ VK_PRESENT_MODE_FIFO_KHR };
    for (u32 i = 0; i < context->device.swapchain_support.present_mode_count; ++i)
    {
        const VkPresentModeKHR mode{ context->device.swapchain_support.present_modes[i] };
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    device::query_swapchain_support(context->device.physical, context->surface, &context->device.swapchain_support);

    const VkSurfaceCapabilitiesKHR& capabilities{ context->device.swapchain_support.capabilities };

    if (capabilities.currentExtent.width != u32_max)
    {
        swapchain_extent = capabilities.currentExtent;
    }

    // Clamp to the value allowed by the GPU.
    const VkExtent2D min{ capabilities.minImageExtent };
    const VkExtent2D max{ capabilities.maxImageExtent };
    swapchain_extent.width  = math::clamp(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = math::clamp(swapchain_extent.height, min.height, max.height);

    u32 image_count{ capabilities.minImageCount + 1 };
    if (capabilities.maxImageCount > 0 && image_count < capabilities.maxImageCount)
    {
        image_count = capabilities.maxImageCount;
    }

    // Swapchain info
    swapchain_info(*context, image_count, out_swapchain, present_mode, swapchain_extent);


    context->current_frame = 0;

    // Images
    create_images(*context, out_swapchain);

    // Views
    create_views(*context, out_swapchain);

    // Depth resources
    if (!device::detect_depth_format(&context->device))
    {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        LOG_FATAL("Failed to find a supported format");
    }

    // Create image and views
    image::create(*context, VK_IMAGE_TYPE_2D, swapchain_extent.width, swapchain_extent.height, context->device.depth_format,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true,
                  VK_IMAGE_ASPECT_DEPTH_BIT, &out_swapchain->depth_attachment);

    LOG_INFO("Swapchain created");
}

void recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_swapchain)
{
    destroy(*context, out_swapchain);
    create(context, width, height, out_swapchain);
}

void destroy(const vulkan_context& context, vulkan_swapchain* swapchain)
{
    vkDeviceWaitIdle(context.device.logical);
    image::destroy(context, &swapchain->depth_attachment);

    for (u32 i = 0; i < swapchain->image_count; ++i)
    {
        vkDestroyImageView(context.device.logical, swapchain->views[i], context.allocator);
    }

    vkDestroySwapchainKHR(context.device.logical, swapchain->handle, context.allocator);

    //if (swapchain->images)
    //{
    //    memory::free(swapchain->images, sizeof(VkImage) * swapchain->image_count, memory_tag::renderer);
    //}

    //if (swapchain->views)
    //{
    //    memory::free(swapchain->views, sizeof(VkImageView) * swapchain->image_count, memory_tag::renderer);
    //}

    //memory::free_(swapchain->framebuffers, memory_tag::renderer, swapchain->image_count);
}

bool acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout,
                              VkSemaphore image_available_semaphore, VkFence fence, u32* out_image_index)
{
    const VkResult result{ vkAcquireNextImageKHR(context->device.logical, swapchain->handle, timeout, image_available_semaphore,
                                                 fence, out_image_index) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // Recreate swapchain and return out of render loop
        recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return false;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_FATAL("Failed to acquire swapchain image");
        return false;
    }

    return true;
}

void present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue,
             VkSemaphore render_complete_semaphore, u32 present_image_index)
{
    VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_complete_semaphore;
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &swapchain->handle;
    present_info.pImageIndices      = &present_image_index;
    present_info.pResults           = nullptr;

    const VkResult result{ vkQueuePresentKHR(present_queue, &present_info) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        // Recreate swapchain because it is either out of date, sub optimal, or a framebuffer resize occurred
        recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    } else if (result != VK_SUCCESS)
    {
        LOG_FATAL("Failed to present swapchain image");
    }

    // % max count in order to loop the index and not go out of bounds
    context->current_frame = (context->current_frame + 1) % swapchain->max_frames_in_flight;
}

} // namespace sky::graphics::vk::swapchain
