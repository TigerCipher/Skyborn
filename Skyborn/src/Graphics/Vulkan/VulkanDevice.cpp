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
//  File Name: VulkanDevice.cpp
//  Date File Created: 04/01/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanDevice.h"

#include "Utl/Vector.h"

namespace sky::graphics::vk
{

namespace
{

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

bool physical_device_meets_requirements(VkPhysicalDevice device, VkSurfaceKHR surface,
                                        const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features,
                                        const physcial_device_requirements*   requirements,
                                        physical_device_queue_family_indices* out_queue_info,
                                        swapchain_support_info*               out_swapchain_support)
{
    out_queue_info->graphics = u32_invalid;
    out_queue_info->present  = u32_invalid;
    out_queue_info->compute  = u32_invalid;
    out_queue_info->transfer = u32_invalid;

    if (requirements->discrete_gpu && properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        LOG_INFOF("Device {} is not a discrete GPU. Skipping", properties->deviceName);
        return false;
    }

    u32 queue_family_count{};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    utl::vector<VkQueueFamilyProperties> queue_families{ queue_family_count };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    // See what each queue supports

    u8 min_transfer_score{ u8_max };
    for (u32 i = 0; i < queue_families.size(); ++i)
    {
        u8 transfer_score{};

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out_queue_info->graphics = i;
            ++transfer_score;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out_queue_info->compute = i;
            ++transfer_score;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            if (transfer_score <= min_transfer_score)
            {
                min_transfer_score       = transfer_score;
                out_queue_info->transfer = i;
            }
        }

        VkBool32 supports_present{ VK_FALSE };
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present)
        {
            out_queue_info->present = i;
        }
    }

    LOG_INFO("Graphics | Present | Compute | Transfer | Name");
    LOG_INFOF("       {} |       {} |       {} |        {} | {}  ", (u8) (out_queue_info->graphics != u32_invalid),
              (u8) (out_queue_info->present != u32_invalid), (u8) (out_queue_info->compute != u32_invalid),
              (u8) (out_queue_info->transfer != u32_invalid), properties->deviceName);

    if ((!requirements->graphics || out_queue_info->graphics != u32_invalid) &&
        (!requirements->present || out_queue_info->present != u32_invalid) &&
        (!requirements->compute || out_queue_info->compute != u32_invalid) &&
        (!requirements->transfer || out_queue_info->transfer != u32_invalid))
    {
        LOG_INFO("Device meets queue requirements");
        LOG_TRACEF("Graphics family index: {}", out_queue_info->graphics);
        LOG_TRACEF("Present family index: {}", out_queue_info->present);
        LOG_TRACEF("Compute family index: {}", out_queue_info->compute);
        LOG_TRACEF("Transfer family index: {}", out_queue_info->transfer);

        // Swapchain support
        query_swapchain_support(device, surface, out_swapchain_support);
        if (!out_swapchain_support->format_count || !out_swapchain_support->present_mode_count)
        {
            if (out_swapchain_support->formats)
            {
                memory::free(out_swapchain_support->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support->format_count,
                             memory_tag::renderer);
            }

            if (out_swapchain_support->present_modes)
            {
                memory::free(out_swapchain_support->present_modes,
                             sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count, memory_tag::renderer);
            }

            LOG_INFO("Required swapchain support not present. Skipping device");
            return false;
        }

        // Device extensions
        if (!requirements->extension_names.empty())
        {
            u32 available_extension_count{};
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extension_count, nullptr));
            if (available_extension_count)
            {
                //utl::vector<VkExtensionProperties> available_extensions{ available_extension_count };
                VkExtensionProperties* available_extensions{ (VkExtensionProperties*) memory::allocate(
                    sizeof(VkExtensionProperties) * available_extension_count, memory_tag::renderer) };
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extension_count, available_extensions));
                for (auto extension_name : requirements->extension_names)
                {
                    bool found{ false };
                    for (u32 j = 0; j < available_extension_count; ++j)
                    {
                        if (utl::string_compare(extension_name, available_extensions[j].extensionName))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        LOG_INFOF("Required extension not found ({}). Skipping device", extension_name);
                        memory::free(available_extensions, sizeof(VkExtensionProperties) * available_extension_count,
                                     memory_tag::renderer);
                        return false;
                    }
                }

                memory::free(available_extensions, sizeof(VkExtensionProperties) * available_extension_count,
                             memory_tag::renderer);
            }
        }

        // Sampler anisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy)
        {
            LOG_INFO("Device does not support samplerAnisotropy. Skipping.");
            return false;
        }

        return true;
    }

    return false;
}

bool select_physical_device(vulkan_context* context)
{
    u32 physical_device_count{};
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, nullptr));
    if (!physical_device_count)
    {
        LOG_FATAL("No device found that supports Vulkan");
        return false;
    }


    utl::vector<VkPhysicalDevice> physical_devices{ physical_device_count };
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices.data()));

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

        if (physical_device_meets_requirements(pd, context->surface, &props, &feats, &pdr, &queue_info,
                                               &context->device.swapchain_support))
        {
            LOG_INFOF("Selected device: {}", props.deviceName);
            switch (props.deviceType) // NOLINT(clang-diagnostic-switch-enum)
            {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: LOG_INFO("Unknown GPU type"); break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: LOG_INFO("GPU is integrated"); break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: LOG_INFO("GPU is discrete"); break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: LOG_INFO("GPU is virtual"); break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: LOG_INFO("GPU is CPU"); break;
            default: break;
            }

            LOG_INFOF("GPU driver version: {}.{}.{}", VK_VERSION_MAJOR(props.driverVersion),
                      VK_VERSION_MINOR(props.driverVersion), VK_VERSION_PATCH(props.driverVersion));
            LOG_INFOF("Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
                      VK_VERSION_PATCH(props.apiVersion));

            // memory info
            for (u32 i = 0; i < mem_props.memoryHeapCount; ++i)
            {
                f32 memory_size_gib{ ((f32) mem_props.memoryHeaps[i].size / 1024.0f / 1024.0f / 1024.0f) };
                if (mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    LOG_INFOF("Local GPU memory: {:.2f} GiB", memory_size_gib);
                } else
                {
                    LOG_INFOF("Shared system memory: {:.2f} GiB", memory_size_gib);
                }
            }

            context->device.physical             = pd;
            context->device.graphics_queue_index = queue_info.graphics;
            context->device.present_queue_index  = queue_info.present;
            context->device.transfer_queue_index = queue_info.transfer;
            // Compute if/when needed

            context->device.properties = props;
            context->device.features   = feats;
            context->device.memory     = mem_props;
            break;
        }
    }

    // Verify that a device was selected
    if (!context->device.physical)
    {
        LOG_ERROR("No physical devices that meet the requirements were found");
        return false;
    }

    LOG_INFO("Physical device selected");
    return true;
}

} // anonymous namespace

bool create_device(vulkan_context* context)
{
    if (!select_physical_device(context))
        return false;
    return true;
}

void destroy_device(vulkan_context* context)
{
    // Physical device
    LOG_INFO("Releasing physical device resources...");
    context->device.physical = nullptr;

    if (context->device.swapchain_support.formats)
    {
        memory::free(context->device.swapchain_support.formats,
                     sizeof(VkSurfaceFormatKHR) * context->device.swapchain_support.format_count, memory_tag::renderer);
        context->device.swapchain_support.format_count = 0;
    }

    if (context->device.swapchain_support.present_modes)
    {
        memory::free(context->device.swapchain_support.present_modes,
                     sizeof(VkPresentModeKHR) * context->device.swapchain_support.present_mode_count, memory_tag::renderer);
        context->device.swapchain_support.present_mode_count = 0;
    }

    context->device.graphics_queue_index = u32_invalid;
    context->device.present_queue_index  = u32_invalid;
    context->device.transfer_queue_index = u32_invalid;
}

void query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_info* out_support_info)
{
    // Surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &out_support_info->capabilities));

    // Surface formats

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &out_support_info->format_count, nullptr));

    if (out_support_info->format_count)
    {
        if (!out_support_info->formats)
        {
            out_support_info->formats = (VkSurfaceFormatKHR*) memory::allocate(
                sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, memory_tag::renderer);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &out_support_info->format_count,
                                                      out_support_info->formats));
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &out_support_info->present_mode_count, nullptr));

    if (out_support_info->present_mode_count)
    {
        if (!out_support_info->present_modes)
        {
            out_support_info->present_modes = (VkPresentModeKHR*) memory::allocate(
                sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, memory_tag::renderer);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &out_support_info->present_mode_count,
                                                           out_support_info->present_modes));
    }
}

} // namespace sky::graphics::vk
