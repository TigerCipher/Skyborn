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

namespace sky::graphics::vk::core
{

namespace
{
struct vk_context
{
    VkInstance             instance;
    VkAllocationCallbacks* allocator{ nullptr };
} context;
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
    create_info.pApplicationInfo        = &app_info;
    create_info.enabledExtensionCount   = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount       = 0;
    create_info.ppEnabledLayerNames     = 0;

    VK_CALL(vkCreateInstance(&create_info, context.allocator, &context.instance));
    LOG_INFO("Vulkan instance created");
    // ...

    LOG_INFO("Vulkan backend initialized");
    return true;
}

void shutdown()
{
    LOG_INFO("Shutting Vulkan backend down...");

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
} // namespace sky::graphics::vk::core