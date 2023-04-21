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
// File Name: VkCore.cpp
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkCore.h"

#include "Skyborn/Util/Util.h"
#include "Skyborn/Core/Platform.h"

#ifdef _WIN64
    #include <vulkan/vulkan_win32.h>
#endif

namespace sky::graphics::vk::core
{

namespace
{
struct vk_context
{
    VkInstance             instance;
    VkAllocationCallbacks* allocator{ nullptr };
    vk_device              main_device{};
    VkSurfaceKHR           surface{};
} context;

#ifdef _DEBUG
VkDebugUtilsMessengerEXT debug_messenger{};
#endif

struct physcial_device_requirements
{
    bool                     graphics{ true };
    bool                     present{ true };
    bool                     compute{ false }; // Default false for now as I don't know when/if I'll need it
    bool                     transfer{ true };
    bool                     sampler_anisotropy{ true };
    bool                     discrete_gpu{ true };
    utl::vector<const char*> extension_names{};
};

struct physical_device_queue_family_indices
{
    u32 graphics{};
    u32 present{};
    u32 compute{};
    u32 transfer{};
};

void query_swapchain_support(VkPhysicalDevice pd)
{
    auto& swapchain = context.main_device.swapchain_support;
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, context.surface, &swapchain.capabilities));

    u32 format_count{};
    VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, context.surface, &format_count, nullptr));

    if (format_count)
    {
        if (swapchain.formats.empty())
        {
            swapchain.formats.resize(format_count);
        }

        VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, context.surface, &format_count, swapchain.formats.data()));
    }

    u32 present_modes_count{};
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, context.surface, &present_modes_count, nullptr));
    if (present_modes_count)
    {
        if (swapchain.present_modes.empty())
        {
            swapchain.present_modes.resize(present_modes_count);
        }
        VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, context.surface, &present_modes_count,
                                                          swapchain.present_modes.data()));
    }
}

bool physical_device_meets_requirements(VkPhysicalDevice physical_device, const VkPhysicalDeviceProperties& properties,
                                        const VkPhysicalDeviceFeatures&       features,
                                        const physcial_device_requirements&   requirements,
                                        physical_device_queue_family_indices& queue_info)
{
    queue_info.compute = queue_info.graphics = queue_info.present = queue_info.transfer = -1;

    swapchain_support_info& swapchain_support = context.main_device.swapchain_support;

    if (requirements.discrete_gpu && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        LOG_INFO("Device {} is not a discrete GPU. Skipping", properties.deviceName);
        return false;
    }

    u32 queue_family_count{};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
    utl::heap_array<VkQueueFamilyProperties> queue_families{ queue_family_count };
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    // See what each queue supports
    u8 min_transfer_score = u8_max;
    for (u32 i = 0; i < queue_families.size(); ++i)
    {
        u8 transfer_score{};
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_info.graphics = i;
            ++transfer_score;
        }
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_info.compute = i;
            ++transfer_score;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            if (transfer_score <= min_transfer_score)
            {
                min_transfer_score  = transfer_score;
                queue_info.transfer = i;
            }
        }

        VkBool32 supports_present{ VK_FALSE };
        VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, context.surface, &supports_present));
        if (supports_present)
        {
            queue_info.present = i;
        }
    }

    LOG_INFO("Graphics | Present | Compute | Transfer | Name");
    LOG_INFO("       {} |       {} |       {} |        {} | {}  ", (u8) (queue_info.graphics != -1),
             (u8) (queue_info.present != -1), (u8) (queue_info.compute != -1), (u8) (queue_info.transfer != -1),
             properties.deviceName);
    if ((!requirements.graphics || queue_info.graphics != -1) && (!requirements.present || queue_info.present != -1) &&
        (!requirements.compute || queue_info.compute != -1) && (!requirements.transfer || queue_info.transfer != -1))
    {
        LOG_INFO("Device meets queue requirements");
        LOG_TRACE("Graphics family index: {}", queue_info.graphics);
        LOG_TRACE("Present family index: {}", queue_info.present);
        LOG_TRACE("Compute family index: {}", queue_info.compute);
        LOG_TRACE("Transfer family index: {}", queue_info.transfer);

        query_swapchain_support(physical_device);
        if (swapchain_support.formats.empty() || swapchain_support.present_modes.empty())
        {
            swapchain_support.formats.clear();
            swapchain_support.present_modes.clear();
            LOG_INFO("Required swapchain support not present. Skipping device");
            return false;
        }

        // Device extensions
        if (!requirements.extension_names.empty())
        {
            u32 available_extension_count{};
            VK_CALL(
                vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extension_count, nullptr));
            if (available_extension_count)
            {
                utl::heap_array<VkExtensionProperties> available_extensions{ available_extension_count };
                VK_CALL(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extension_count,
                                                             available_extensions.data()));
                for (const auto& extension_name : requirements.extension_names)
                {
                    LOG_INFO("Searching for extension '{}'...", extension_name);
                    bool found{ false };
                    for (u32 i = 0; i < available_extensions.size(); ++i)
                    {
                        if (utl::string_compare(extension_name, available_extensions[i].extensionName))
                        {
                            found = true;
                            LOG_INFO("Found.");
                            break;
                        }
                    }

                    if (!found)
                    {
                        LOG_INFO("Not found. Skipping device");
                        return false;
                    }
                }
            }
        }

        if (requirements.sampler_anisotropy && !features.samplerAnisotropy)
        {
            LOG_INFO("Device does not support sampler anisotropy. Skipping");
            return false;
        }
        return true;
    }

    return false;
}

bool select_physical_device()
{
    u32 physical_device_count{};
    VK_CALL(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, nullptr));
    if (!physical_device_count)
    {
        LOG_FATAL("No device found that supports Vulkan");
        return false;
    }

    utl::heap_array<VkPhysicalDevice> physical_devices{ physical_device_count };
    VK_CALL(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices.data()));

    for (const auto& pd : physical_devices)
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(pd, &props);

        VkPhysicalDeviceFeatures feats{};
        vkGetPhysicalDeviceFeatures(pd, &feats);

        VkPhysicalDeviceMemoryProperties mem_props{};
        vkGetPhysicalDeviceMemoryProperties(pd, &mem_props);

        physcial_device_requirements pdr{};
        pdr.extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        physical_device_queue_family_indices queue_info{};

        if (physical_device_meets_requirements(pd, props, feats, pdr, queue_info))
        {
            LOG_INFO("Selected device: {}", props.deviceName);
            switch (props.deviceType)
            {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: LOG_INFO("Unknown GPU type"); break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: LOG_INFO("GPU is integrated"); break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: LOG_INFO("GPU is discrete"); break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: LOG_INFO("GPU is virtual"); break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: LOG_INFO("GPU is CPU"); break;
            default: break;
            }
            LOG_INFO("GPU driver version: {}.{}.{}", VK_VERSION_MAJOR(props.driverVersion),
                     VK_VERSION_MINOR(props.driverVersion), VK_VERSION_PATCH(props.driverVersion));
            LOG_INFO("Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(props.apiVersion),
                     VK_VERSION_MINOR(props.apiVersion), VK_VERSION_PATCH(props.apiVersion));

            // memory info
            for (u32 i = 0; i < mem_props.memoryHeapCount; ++i)
            {
                f32 memory_size_gib = (f32) mem_props.memoryHeaps[i].size / 1024.0f / 1024.0f / 1024.0f;
                if (mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    LOG_INFO("Local GPU memory: {:.2f} GiB", memory_size_gib);
                } else
                {
                    LOG_INFO("Shared system memory: {:.2f} GiB", memory_size_gib);
                }
            }
            vk_device& dev           = context.main_device;
            dev.physical_device      = pd;
            dev.graphics_queue_index = queue_info.graphics;
            dev.present_queue_index  = queue_info.present;
            dev.transfer_queue_index = queue_info.transfer;
            // TODO: Compute
            dev.properties = props;
            dev.features   = feats;
            dev.memory     = mem_props;
            break;
        }
    }

    if (!context.main_device.physical_device)
    {
        LOG_ERROR("No suitable physical device found");
        return false;
    }

    LOG_INFO("Physical device selected");
    return true;
}

// Debug callback
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                              VkDebugUtilsMessageTypeFlagsEXT             message_types,
                                              const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                              void*                                       user_data)
{
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: LOG_ERROR("Vulkan Error: {}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARN("Vulkan Warning: {}", callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: LOG_INFO("Vulkan Info: {}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_TRACE("Vulkan Verbose Info: {}", callback_data->pMessage);
        break;
    default: break;
    }

    return VK_FALSE;
}

bool create_surface()
{
    VkWin32SurfaceCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hinstance = platform::get_window_instance();
    create_info.hwnd      = platform::get_window_handle();

    if (vkCreateWin32SurfaceKHR(context.instance, &create_info, context.allocator, &context.surface) != VK_SUCCESS)
    {
        LOG_FATAL("Vulkan surface creation failed");
        return false;
    }

    return true;
}

bool create_device()
{
    if (!select_physical_device())
    {
        return false;
    }
    return true;
}

void destroy_device()
{
    LOG_INFO("Releasing physical device resources");
    context.main_device.physical_device = nullptr;
    context.main_device.swapchain_support.formats.clear();
    context.main_device.swapchain_support.present_modes.clear();
    context.main_device.graphics_queue_index = -1;
    context.main_device.present_queue_index  = -1;
    context.main_device.transfer_queue_index = -1;
}

bool detect_depth_format()
{
    return true;
}


} // anonymous namespace

bool initialize(const char* app_name)
{
    LOG_INFO("Initializing Vulkan graphics backend");

    VkApplicationInfo app_info  = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion         = VK_API_VERSION_1_2;
    app_info.pApplicationName   = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Skyborn Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;

    utl::vector<const char*> required_extensions{};
    required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if SKY_PLATFORM_WINDOWS
    required_extensions.push_back("VK_KHR_win32_surface");
#endif

#ifdef _DEBUG
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    LOG_DEBUG("Required vulkan extensions:");
    for (const auto& ext : required_extensions)
    {
        LOG_DEBUG("{}", ext);
    }
#endif

    create_info.enabledExtensionCount   = (u32) required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    utl::vector<const char*> validation_layers{};

#ifdef _DEBUG
    LOG_DEBUG("Validation layers enabled");
    validation_layers.push_back("VK_LAYER_KHRONOS_validation");

    u32 available_layer_count{};
    VK_CALL(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
    utl::heap_array<VkLayerProperties> available_layers{ available_layer_count };
    VK_CALL(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));

    for (const auto& required_layer : validation_layers)
    {
        LOG_DEBUG("Searching for layer: {}...", required_layer);
        bool found = false;
        for (u32 i = 0; i < available_layer_count; ++i)
        {
            if (utl::string_compare(required_layer, available_layers[i].layerName))
            {
                found = true;
                LOG_DEBUG("Found.");
                break;
            }
        }
        if (!found)
        {
            LOG_FATAL("Required validation layer is missing: {}", required_layer);
            return false;
        }
    }

    LOG_DEBUG("All required validation layers are present");

#endif

    create_info.enabledLayerCount   = (u32) validation_layers.size();
    create_info.ppEnabledLayerNames = validation_layers.data();

    VK_CALL(vkCreateInstance(&create_info, context.allocator, &context.instance));
    LOG_INFO("Vulkan instance created");

// Debugger
#ifdef _DEBUG
    LOG_DEBUG("Creating Vulkan debugger");
    constexpr u32 log_severity{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT };
    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_callback;

    const auto func =
        (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    assert_msg(func, "Failed to create vulkan debug messenger");
    VK_CALL(func(context.instance, &debug_create_info, context.allocator, &debug_messenger));
    LOG_DEBUG("Vulkan debugger created");
#endif

    // Surface
    LOG_INFO("Creating Vulkan surface");
    if (!create_surface())
    {
        LOG_ERROR("Failed to create Vulkan surface");
        return false;
    }

    // Device
    if (!create_device())
    {
        LOG_ERROR("Failed to create Vulkan device");
        return false;
    }

    LOG_INFO("Vulkan backend initialized");
    return true;
}

void shutdown()
{
    // NOTE: Resources should be destroyed in reverse order of creation

    LOG_INFO("Shutting Vulkan backend down...");

    destroy_device();

    if (context.surface)
    {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = nullptr;
    }


#ifdef _DEBUG
    LOG_DEBUG("Destroying Vulkan debugger");
    if (debug_messenger)
    {
        const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, debug_messenger, context.allocator);
    }
#endif

    LOG_DEBUG("Destroying Vulkan instance");
    vkDestroyInstance(context.instance, context.allocator);
}

void resized(u16 width, u16 height) {}

bool begin_frame(f32 delta)
{
    return true;
}

bool end_frame(f32 delta)
{
    return true;
}

vk_device& device()
{
    return context.main_device;
}

} // namespace sky::graphics::vk::core