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

#include "Utl/Vector.h"
#include "Utl/HeapArray.h"

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

    VkCommandPool graphics_cmd_pool{};

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

struct vulkan_render_pass_state
{
    enum state : u8
    {
        ready,
        recording,
        in_render_pass,
        recording_ended,
        submitted,
        not_allocated
    };
};

struct vulkan_renderpass
{
    VkRenderPass handle{};

    f32 x{}, y{}, w{}, h{};
    f32 r{}, g{}, b{}, a{};
    f32 depth{};
    u32 stencil{};

    vulkan_render_pass_state::state state;
};

struct vulkan_framebuffer
{
    VkFramebuffer      handle{};
    vulkan_renderpass* renderpass{};

    //utl::vector<VkImageView, memory_tag::renderer> attachments{};
    //VkImageView* attachments{}; // never resizes, I think it will be faster to not use a vector for this
    utl::heap_array<VkImageView> attachments{};
    u32                          attachment_count{};
};

struct vulkan_swapchain
{
    VkSurfaceFormatKHR           image_format{};
    u8                           max_frames_in_flight{};
    VkSwapchainKHR               handle{};
    u32                          image_count{};
    utl::heap_array<VkImage>     images{};
    utl::heap_array<VkImageView> views{};

    vulkan_image depth_attachment{};

    //utl::vector<vulkan_framebuffer, memory_tag::renderer> framebuffers{};
    //vulkan_framebuffer* framebuffers{}; //  Won't resize, better off a light weight array than a vector
    utl::heap_array<vulkan_framebuffer> framebuffers{};
};

struct vulkan_command_buffer_state
{
    enum state : u8
    {
        ready,
        recording,
        in_render_pass,
        recording_ended,
        submitted,
        not_allocated
    };
};

struct vulkan_command_buffer
{
    VkCommandBuffer handle{};

    vulkan_command_buffer_state::state state{};
};

struct vulkan_fence
{
    VkFence handle{};
    bool    is_signaled{};
};

struct vulkan_context
{
    VkInstance             instance{ nullptr };
    VkAllocationCallbacks* allocator{ nullptr };
    VkSurfaceKHR           surface{};

    u32 framebuffer_width{};
    u32 framebuffer_height{};

    u64 framebuffer_size_generation{};
    u64 framebuffer_size_last_generation{};

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger{};
#endif

    vulkan_device     device{};
    vulkan_swapchain  swapchain{};
    vulkan_renderpass main_renderpass{};

    utl::vector<vulkan_command_buffer, memory_tag::renderer> graphics_cmd_buffers{};

    //utl::vector<VkSemaphore, memory_tag::renderer> image_available_semaphores{};
    //utl::vector<VkSemaphore, memory_tag::renderer> queue_complete_semaphores{};
    utl::heap_array<VkSemaphore> queue_complete_semaphores{};
    utl::heap_array<VkSemaphore> image_available_semaphores{};

    u32 in_flight_fence_count{};

    //utl::vector<vulkan_fence, memory_tag::renderer> in_flight_fences{};
    utl::heap_array<vulkan_fence> in_flight_fences{};

    //utl::vector<vulkan_fence*, memory_tag::renderer> images_in_flight{};
    utl::heap_array<vulkan_fence*> images_in_flight{};

    u32 image_index{};
    u32 current_frame{};

    bool recreating_swapchain{};

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags){};
};

inline bool is_success(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS:
    case VK_NOT_READY:
    case VK_TIMEOUT:
    case VK_EVENT_SET:
    case VK_EVENT_RESET:
    case VK_INCOMPLETE:
    case VK_SUBOPTIMAL_KHR:
    case VK_THREAD_IDLE_KHR:
    case VK_THREAD_DONE_KHR:
    case VK_OPERATION_DEFERRED_KHR:
    case VK_OPERATION_NOT_DEFERRED_KHR:
    case VK_PIPELINE_COMPILE_REQUIRED_EXT: return true;
    default: return false;
    }
}


} // namespace sky::graphics::vk