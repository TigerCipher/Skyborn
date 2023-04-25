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
// File Name: VkFence.cpp
// Date File Created: 04/23/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkFence.h"
#include "VkCore.h"

namespace sky::graphics::vk::fence
{
vk_fence create(bool create_signaled)
{
    vk_fence fence{};
    fence.is_signaled = create_signaled;
    VkFenceCreateInfo fence_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    if (create_signaled)
    {
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VK_CALL(vkCreateFence(core::logical_device(), &fence_info, core::allocator(), &fence.handle));
    return fence;
}

void destroy(vk_fence& fence)
{
    if (fence.handle)
    {
        vkDestroyFence(core::logical_device(), fence.handle, core::allocator());
        fence.handle = nullptr;
    }
    fence.is_signaled = false;
}

bool wait(vk_fence& fence, u64 timeout)
{
    if (!fence.is_signaled)
    {
        switch (vkWaitForFences(core::logical_device(), 1, &fence.handle, VK_TRUE, timeout))
        {
        case VK_SUCCESS: fence.is_signaled = true; return true;
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

void reset(vk_fence& fence)
{
    if (fence.is_signaled)
    {
        vkResetFences(core::logical_device(), 1, &fence.handle);
        fence.is_signaled = false;
    }
}
} // namespace sky::graphics::vk::fence