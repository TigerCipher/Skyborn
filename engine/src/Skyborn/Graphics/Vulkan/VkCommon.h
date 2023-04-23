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
// File Name: VkCommon.h
// Date File Created: 04/19/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Skyborn/Defines.h"
#include "Skyborn/Debug/Logger.h"
#include "Skyborn/Debug/Asserts.h"
#include "Skyborn/Graphics/Renderer.h"
#include "Skyborn/Util/Vector.h"
#include "Skyborn/Util/HeapArray.h"

#include <vulkan/vulkan.h>

#ifdef _DEBUG
    #define VK_CALL(expr)                                                                                              \
        if ((expr) != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOG_ERROR("Vulkan Call Error > {}", #expr);                                                                \
            LOG_ERROR("In {} at line {}", __FILE__, __LINE__);                                                         \
            sky_assert(false);                                                                                         \
        }
#else
    #define VK_CALL(expr) expr
#endif

namespace sky::graphics::vk
{
struct swapchain_support_info
{
    VkSurfaceCapabilitiesKHR        capabilities{};
    utl::vector<VkSurfaceFormatKHR> formats{};
    utl::vector<VkPresentModeKHR>   present_modes{};
};
} // namespace sky::graphics::vk
