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
#include "VkSurface.h"

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
    vk_surface             surface{};
} context;

utl::vector<vk_command_buffer> command_buffers{};

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

bool physical_device_meets_requirements(VkPhysicalDevice physical_device, const VkPhysicalDeviceProperties& properties,
                                        const VkPhysicalDeviceFeatures&       features,
                                        const physcial_device_requirements&   requirements,
                                        physical_device_queue_family_indices& queue_info, VkSurfaceKHR surface)
{
    queue_info.compute = queue_info.graphics = queue_info.present = queue_info.transfer = -1;

    swapchain_support_info swapchain_support{};

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
        VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &supports_present));
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

        swapchain_support = get_swapchain_support_info(physical_device, surface);
        // query_swapchain_support(physical_device, swapchain_support);
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

bool select_physical_device(VkSurfaceKHR surface)
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

        if (physical_device_meets_requirements(pd, props, feats, pdr, queue_info, surface))
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

void destroy_device()
{
    LOG_INFO("Destroying logical device");
    context.main_device.graphics_queue = nullptr;
    context.main_device.present_queue  = nullptr;
    context.main_device.transfer_queue = nullptr;

    LOG_INFO("Destroying command pools");
    vkDestroyCommandPool(context.main_device.logical_device, context.main_device.command_pool, context.allocator);

    if (context.main_device.logical_device)
    {
        vkDestroyDevice(context.main_device.logical_device, context.allocator);
        context.main_device.logical_device = nullptr;
    }

    LOG_INFO("Releasing physical device resources");
    context.main_device.physical_device = nullptr;
    // context.main_device.swapchain_support.formats.clear();
    // context.main_device.swapchain_support.present_modes.clear();
    context.main_device.graphics_queue_index = -1;
    context.main_device.present_queue_index  = -1;
    context.main_device.transfer_queue_index = -1;
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

    // Surface (and device)
    context.surface.create();

    LOG_INFO("Vulkan backend initialized");
    return true;
}

void shutdown()
{
    // NOTE: Resources should be destroyed in reverse order of creation
    // This is not a hard requirement, but does help ensure nothing is destroyed that is still in use

    LOG_INFO("Shutting Vulkan backend down...");

    context.surface.destroy();

    destroy_device();


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

bool create_device(VkSurfaceKHR surface)
{
    if (!select_physical_device(surface))
    {
        return false;
    }

    LOG_INFO("Creating logical device");
    const bool present_shares_graphics_queue =
        context.main_device.graphics_queue_index == context.main_device.present_queue_index;
    const bool transfer_shares_graphics_queue =
        context.main_device.graphics_queue_index == context.main_device.transfer_queue_index;
    utl::vector<u32> indices{};
    indices.push_back(context.main_device.graphics_queue_index);
    if (!present_shares_graphics_queue)
    {
        indices.push_back(context.main_device.present_queue_index);
    }
    if (!transfer_shares_graphics_queue)
    {
        indices.push_back(context.main_device.transfer_queue_index);
    }

    utl::heap_array<VkDeviceQueueCreateInfo> queue_create_infos{ indices.size() };

    for (u32 i = 0; i < indices.size(); ++i)
    {
        auto& [sType, pNext, flags, queueFamilyIndex, queueCount, pQueuePriorities]{ queue_create_infos[i] };
        sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueFamilyIndex = indices[i];
        queueCount       = 1;
        if (indices[i] == context.main_device.graphics_queue_index)
        {
            queueCount = 2;
        }

        flags = 0;
        pNext = nullptr;
        f32 queue_priority[2]{ 1.0f, 1.0f };
        pQueuePriorities = queue_priority;
    }

    // TODO: Make configurable
    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    create_info.queueCreateInfoCount = indices.size();
    create_info.pQueueCreateInfos    = queue_create_infos.data();

    create_info.pEnabledFeatures        = &device_features;
    create_info.enabledExtensionCount   = 1;
    auto ext_names                      = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    create_info.ppEnabledExtensionNames = &ext_names;

    // Explicitly zeroing deprecated items
    create_info.enabledLayerCount   = 0;
    create_info.ppEnabledLayerNames = nullptr;

    VK_CALL(vkCreateDevice(context.main_device.physical_device, &create_info, context.allocator,
                           &context.main_device.logical_device));
    LOG_INFO("Logical device created");
    vkGetDeviceQueue(context.main_device.logical_device, context.main_device.graphics_queue_index, 0,
                     &context.main_device.graphics_queue);
    vkGetDeviceQueue(context.main_device.logical_device, context.main_device.present_queue_index, 0,
                     &context.main_device.present_queue);
    vkGetDeviceQueue(context.main_device.logical_device, context.main_device.transfer_queue_index, 0,
                     &context.main_device.transfer_queue);
    LOG_INFO("Obtained device queues");

    VkCommandPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    pool_info.queueFamilyIndex = context.main_device.graphics_queue_index;
    pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL(vkCreateCommandPool(context.main_device.logical_device, &pool_info, context.allocator,
                                &context.main_device.command_pool));

    return true;
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

bool detect_depth_format()
{
    constexpr u8 candidate_count{ 3 };
    VkFormat     candidates[candidate_count]{
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    constexpr u32 flags{ VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT };

    for (const auto& candidate : candidates)
    {
        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties(context.main_device.physical_device, candidate, &props);

        if ((props.linearTilingFeatures & flags) == flags || (props.optimalTilingFeatures & flags) == flags)
        {
            context.main_device.depth_format = candidate;
            return true;
        }
    }

    return false;
}

vk_device& device()
{
    return context.main_device;
}

VkPhysicalDevice physical_device()
{
    return context.main_device.physical_device;
}

VkDevice logical_device()
{
    return context.main_device.logical_device;
}

VkAllocationCallbacks* allocator()
{
    return context.allocator;
}

VkInstance instance()
{
    return context.instance;
}

VkSurfaceKHR surface()
{
    return context.surface.handle();
}

u32 find_memory_index(u32 type_filter, u32 property_flags)
{
    VkPhysicalDeviceMemoryProperties props{};
    vkGetPhysicalDeviceMemoryProperties(context.main_device.physical_device, &props);
    for (u32 i = 0; i < props.memoryTypeCount; ++i)
    {
        if (type_filter & (1 << i) && (props.memoryTypes[i].propertyFlags & property_flags) == property_flags)
        {
            return i;
        }
    }

    LOG_WARN("Unable to find a suitable memory type");
    return u32_invalid;
}

} // namespace sky::graphics::vk::core