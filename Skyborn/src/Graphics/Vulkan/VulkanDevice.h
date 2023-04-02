﻿//  ------------------------------------------------------------------------------
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
//  File Name: VulkanDevice.h
//  Date File Created: 04/01/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#pragma once

#include "VulkanCommon.h"

namespace sky::graphics::vk
{

bool create_device(vulkan_context* context);
void destroy_device(vulkan_context* context);

void query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_info* out_support_info);

}