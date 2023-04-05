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
//  File Name: VulkanBackend.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanBackend.h"
#include "VulkanCommon.h"
#include "VulkanPlatform.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanFence.h"

#include "Core/Application.h"

namespace sky::graphics::vk
{

namespace
{
vulkan_context context{};

struct framebuffer_cache
{
    u32 width{};
    u32 height{};
} fb_cache;

// Debug callback
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT             message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
    switch (message_severity) // NOLINT(clang-diagnostic-switch-enum)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: LOG_ERRORF("Vulkan Error: {}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: LOG_WARNF("Vulkan Warning: {}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: LOG_INFOF("Vulkan Info: {}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: LOG_TRACEF("Vulkan Verbose Info: {}", callback_data->pMessage); break;
    default: break;
    }

    return VK_FALSE;
}

i32 find_memory_index(u32 type_filter, u32 property_flags)
{
    VkPhysicalDeviceMemoryProperties props{};
    vkGetPhysicalDeviceMemoryProperties(context.device.physical, &props);
    for (u32 i = 0; i < props.memoryTypeCount; ++i)
    {
        if (type_filter & (1 << i) && (props.memoryTypes[i].propertyFlags & property_flags) == property_flags)
        {
            return i;
        }
    }

    LOG_WARN("Unable to find a suitable memory type");
    return -1;
}

void create_command_buffers()
{
    if(context.graphics_cmd_buffers.empty())
    {
        context.graphics_cmd_buffers.resize(context.swapchain.image_count);
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i)
    {
        if(context.graphics_cmd_buffers[i].handle)
        {
            commands::free_buffer(context, context.device.graphics_cmd_pool, &context.graphics_cmd_buffers[i]);
        }
        //context.graphics_cmd_buffers[i] = {};
        commands::allocate_buffer(context, context.device.graphics_cmd_pool, true, &context.graphics_cmd_buffers[i]);
    }

    LOG_INFO("Vulkan command buffers created");
}

void regenerate_framebuffers(vulkan_swapchain* swapchain, vulkan_renderpass* renderpass)
{
    constexpr u32 attachment_count{2}; // TODO make configurable / dynamic
    for (u32 i = 0; i < swapchain->image_count; ++i)
    {
        const VkImageView attachments[attachment_count]{swapchain->views[i], swapchain->depth_attachment.view};

        framebuffer::create(context, renderpass, context.framebuffer_width, context.framebuffer_height, attachment_count, attachments, &context.swapchain.framebuffers[i]);
    }
}

} // anonymous namespace

bool initialize(const char* app_name)
{
    context.find_memory_index = find_memory_index;

    app::get_framebuffer_size(&fb_cache.width, &fb_cache.height);
    context.framebuffer_width = fb_cache.width ? fb_cache.width : 800;
    context.framebuffer_height = fb_cache.height ? fb_cache.height : 600;
    fb_cache.width = 0;
    fb_cache.height = 0;


    VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion         = VK_API_VERSION_1_2;
    app_info.pApplicationName   = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Skyborn Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;


    utl::vector<const char*> required_extensions{};
    required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface
    platform::get_required_extensions(required_extensions);       // Any platform specific extensions

    // Only add the debug utils extension in debug mode
#ifdef _DEBUG
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Vulkan debug support

    LOG_DEBUG("Required vulkan extensions:");
    for (const auto& ext : required_extensions)
    {
        LOG_DEBUG(ext);
    }
#endif

    create_info.enabledExtensionCount   = (u32) required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    utl::vector<const char*> required_validation_layers{};
    u32                      validation_layers_count{};

    // Only enable validation layers in debug mode
#ifdef _DEBUG

    LOG_DEBUG("Validation layers enabled...");

    // Required validation layers
    required_validation_layers.push_back("VK_LAYER_KHRONOS_validation");
    validation_layers_count = (u32) required_validation_layers.size();

    // Get a list of available validation layers
    u32 available_layer_count{};
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
    utl::vector<VkLayerProperties> available_layers{ available_layer_count };
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));

    // Verify that all required validation layers are present
    for (auto& required_validation_layer : required_validation_layers)
    {
        LOG_DEBUGF("Searching for layer: {}...", required_validation_layer);
        bool found{ false };
        for (u32 j = 0; j < available_layer_count; ++j)
        {
            if (utl::string_compare(required_validation_layer, available_layers[j].layerName))
            {
                found = true;
                LOG_DEBUG("Found.");
                break;
            }
        }

        if (!found)
        {
            LOG_FATALF("Required validation layer is missing: {}", required_validation_layer);
            return false;
        }
    }

    LOG_DEBUG("All required validation layers are present");
#endif


    create_info.enabledLayerCount   = validation_layers_count;
    create_info.ppEnabledLayerNames = required_validation_layers.data();

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    LOG_INFO("Vulkan instance created");

// Debugger
#ifdef _DEBUG

    LOG_DEBUG("Creating Vulkan debugger");
    constexpr u32 log_severity{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT };
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;

    const auto func{ (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance,
                                                                                "vkCreateDebugUtilsMessengerEXT") };
    SKY_ASSERT(func, "Failed to create vulkan debug messenger");
    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    LOG_DEBUG("Vulkan debugger created");

#endif


    LOG_DEBUG("Creating Vulkan surface");
    if (!platform::create_surface(&context))
    {
        LOG_ERROR("Failed to create vulkan surface");
        return false;
    }

    if (!device::create(&context))
    {
        LOG_ERROR("Failed to create Vulkan device");
        return false;
    }


    swapchain::create(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);
    renderpass::create(&context, &context.main_renderpass, 0.0f, 0.0f, (f32) context.framebuffer_width,
                       (f32) context.framebuffer_height, 0.0f, 0.0f, 0.2f, 0.1f, 1.0f, 0.0f);

    // Swapchain framebuffers
    //context.swapchain.framebuffers.resize(context.swapchain.image_count);
    memory::allocate(context.swapchain.framebuffers, memory_tag::renderer, context.swapchain.image_count);
    regenerate_framebuffers(&context.swapchain, &context.main_renderpass);

    create_command_buffers();

    // Sync objects
    context.image_available_semaphores.resize(context.swapchain.max_frames_in_flight);
    context.queue_complete_semaphores.resize(context.swapchain.max_frames_in_flight);
    context.in_flight_fences.resize(context.swapchain.max_frames_in_flight);

    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i)
    {
        VkSemaphoreCreateInfo sem_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(context.device.logical, &sem_info, context.allocator, &context.image_available_semaphores[i]);
        vkCreateSemaphore(context.device.logical, &sem_info, context.allocator, &context.queue_complete_semaphores[i]);

        fence::create(context, true, &context.in_flight_fences[i]);
    }

    context.images_in_flight.resize(context.swapchain.image_count, nullptr); // just to be safe, filling the vector with nullptr


    LOG_INFO("Vulkan backend initialized");
    return true;
}

void shutdown()
{
    vkDeviceWaitIdle(context.device.logical);
    // Note: Destroy resources in reverse order of creation

    // Sync objects
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i)
    {
        if(context.image_available_semaphores[i])
        {
            vkDestroySemaphore(context.device.logical, context.image_available_semaphores[i], context.allocator);
        }
        if(context.queue_complete_semaphores[i])
        {
            vkDestroySemaphore(context.device.logical, context.queue_complete_semaphores[i], context.allocator);
        }
        fence::destroy(context, &context.in_flight_fences[i]);
    }

    context.image_available_semaphores.clear();
    context.queue_complete_semaphores.clear();
    context.in_flight_fences.clear();

    // Command buffers
    for (u32 i = 0; i < context.swapchain.image_count; ++i)
    {
        if(context.graphics_cmd_buffers[i].handle)
        {
            commands::free_buffer(context, context.device.graphics_cmd_pool, &context.graphics_cmd_buffers[i]);
            context.graphics_cmd_buffers[i].handle = nullptr;
        }
    }

    context.graphics_cmd_buffers.clear();

    for (u32 i = 0; i < context.swapchain.image_count; ++i)
    {
        framebuffer::destroy(context, &context.swapchain.framebuffers[i]);
    }

    renderpass::destroy(context, &context.main_renderpass);
    swapchain::destroy(context, &context.swapchain);

    LOG_DEBUG("Destroying vulkan device");
    device::destroy(&context);

    LOG_DEBUG("Destroying vulkan surface");
    if (context.surface)
    {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = nullptr;
    }

#ifdef _DEBUG
    LOG_DEBUG("Destroying Vulkan debugger");
    if (context.debug_messenger)
    {
        const auto func{ (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance,
                                                                                     "vkDestroyDebugUtilsMessengerEXT") };
        func(context.instance, context.debug_messenger, context.allocator);
    }
#endif

    LOG_DEBUG("Destroying Vulkan instance");
    vkDestroyInstance(context.instance, context.allocator);
}

void on_resized(u16 width, u16 height) {}

bool begin_frame(f32 delta)
{
    return true;
}

bool end_frame(f32 delta)
{
    return true;
}

} // namespace sky::graphics::vk
