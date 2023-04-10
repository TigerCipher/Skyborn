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
//  File Name: VulkanFence.cpp
//  Date File Created: 04/03/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanFence.h"

namespace sky::graphics::vk::fence
{
void create(const vulkan_context& context, bool create_signaled, vulkan_fence* pfence)
{
    pfence->is_signaled = create_signaled;

    VkFenceCreateInfo fence_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    if (create_signaled)
    {
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VK_CHECK(vkCreateFence(context.device.logical, &fence_info, context.allocator, &pfence->handle));
}

void destroy(const vulkan_context& context, vulkan_fence* pfence)
{
    if (pfence->handle)
    {
        vkDestroyFence(context.device.logical, pfence->handle, context.allocator);
        pfence->handle = nullptr;
    }
    pfence->is_signaled = false;
}

bool wait(const vulkan_context& context, vulkan_fence* pfence, u64 timeout)
{
    if (!pfence->is_signaled)
    {
        switch (vkWaitForFences(context.device.logical, 1, &pfence->handle, VK_TRUE, timeout))
        {
        case VK_SUCCESS: pfence->is_signaled = true; return true;
        case VK_TIMEOUT: LOG_WARN("vk_fence_wait - Timed out"); break;
        case VK_ERROR_DEVICE_LOST: LOG_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST."); break;
        case VK_ERROR_OUT_OF_HOST_MEMORY: LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY."); break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY."); break;
        default: LOG_ERROR("vk_fence_wait - An unknown error has occurred."); break;
        }
    } else
        return true;

    return false;
}

void reset(const vulkan_context& context, vulkan_fence* pfence)
{
    if (pfence->is_signaled)
    {
        vkResetFences(context.device.logical, 1, &pfence->handle);
        pfence->is_signaled = false;
    }
}

} // namespace sky::graphics::vk::fence
