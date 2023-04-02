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

namespace sky::graphics::vk
{

namespace
{
vulkan_context context{};

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

} // anonymous namespace

bool initialize(const char* app_name)
{
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

    if (!create_device(&context))
    {
        LOG_ERROR("Failed to create Vulkan device");
        return false;
    }

    LOG_INFO("Vulkan backend initialized");
    return true;
}

void shutdown()
{
    LOG_DEBUG("Destroying vulkan device");
    destroy_device(&context);

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
