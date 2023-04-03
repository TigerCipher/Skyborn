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
//  File Name: VulkanCommon.h
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"

#include <vulkan/vulkan.h>

#ifdef _DEBUG
    #define VK_CHECK(expr) assert((expr) == VK_SUCCESS)
#else
    #define VK_CHECK(expr) expr
#endif

namespace sky::graphics::vk
{

struct swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities{};
    u32                      format_count{};
    VkSurfaceFormatKHR*      formats{};
    u32                      present_mode_count{};
    VkPresentModeKHR*        present_modes{};
};

// represents both the physical and logical devices
struct vulkan_device
{
    VkPhysicalDevice       physical{};
    VkDevice               logical{};
    swapchain_support_info swapchain_support{};

    u32     graphics_queue_index{};
    u32     present_queue_index{};
    u32     transfer_queue_index{};
    VkQueue graphics_queue{};
    VkQueue present_queue{};
    VkQueue transfer_queue{};

    VkPhysicalDeviceProperties       properties{};
    VkPhysicalDeviceFeatures         features{};
    VkPhysicalDeviceMemoryProperties memory{};

    VkFormat depth_format{};
};

struct vulkan_image
{
    VkImage        handle{};
    VkDeviceMemory memory{};
    VkImageView    view{};
    u32            width{};
    u32            height{};
};

struct vulkan_swapchain
{
    VkSurfaceFormatKHR image_format{};
    u8                 max_frames_in_flight{};
    VkSwapchainKHR     handle{};
    u32                image_count{};
    VkImage*           images{};
    VkImageView*       views{};

    vulkan_image depth_attachment{};
};

struct vulkan_context
{
    VkInstance             instance{ nullptr };
    VkAllocationCallbacks* allocator{ nullptr };
    VkSurfaceKHR           surface{};

    u32 framebuffer_width{};
    u32 framebuffer_height{};

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger{};
#endif

    vulkan_device    device{};
    vulkan_swapchain swapchain{};
    u32              image_index{};
    u32              current_frame{};

    bool recreating_swapchain{};

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
};


} // namespace sky::graphics::vk