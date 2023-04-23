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
// File Name: VkCore.h
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "VkCommon.h"

namespace sky::graphics::vk::core
{


// TODO: Might move device stuff to its own file - and maybe as a class?
struct vk_device
{
    VkPhysicalDevice physical_device{};
    VkDevice         logical_device{};
    // swapchain_support_info           swapchain_support{};
    i32                              graphics_queue_index{};
    i32                              present_queue_index{};
    i32                              transfer_queue_index{};
    VkQueue                          graphics_queue{};
    VkQueue                          present_queue{};
    VkQueue                          transfer_queue{};
    VkPhysicalDeviceProperties       properties{};
    VkPhysicalDeviceFeatures         features{};
    VkPhysicalDeviceMemoryProperties memory{};
    VkFormat                         depth_format{};
};

struct framebuffer_info
{
    // u32 width{};
    // u32 height{};
    u32 current_frame{};
};

bool initialize(const char* app_name);

void shutdown();
void resized(u16 width, u16 height);
bool begin_frame(f32 delta);
bool end_frame(f32 delta);
bool detect_depth_format();
bool create_device(VkSurfaceKHR surface);

VkInstance             instance();
vk_device&             device();
VkPhysicalDevice       physical_device();
VkDevice               logical_device();
VkAllocationCallbacks* allocator();
VkSurfaceKHR           surface();

u32 find_memory_index(u32 type_filter, u32 property_flags);

framebuffer_info& framebuffer();

} // namespace sky::graphics::vk::core