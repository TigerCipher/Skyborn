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
//  File Name: VulkanCommandBuffer.cpp
//  Date File Created: 04/03/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanCommandBuffer.h"

namespace sky::graphics::vk::commands
{
void allocate_buffer(const vulkan_context& context, VkCommandPool pool, bool is_primary, vulkan_command_buffer* pbuffer)
{
    *pbuffer = {}; // not really needed, but just to be safe I want to make sure the buffer is fresh

    VkCommandBufferAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    alloc_info.commandPool        = pool;
    alloc_info.level              = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    alloc_info.commandBufferCount = 1;
    alloc_info.pNext              = nullptr;

    pbuffer->state = vulkan_command_buffer_state::not_allocated;
    VK_CHECK(vkAllocateCommandBuffers(context.device.logical, &alloc_info, &pbuffer->handle));
    pbuffer->state = vulkan_command_buffer_state::ready;
}

void free_buffer(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer)
{
    vkFreeCommandBuffers(context.device.logical, pool, 1, &pbuffer->handle);
    pbuffer->handle = nullptr;
    pbuffer->state  = vulkan_command_buffer_state::not_allocated;
}

void begin(vulkan_command_buffer* pbuffer, bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use)
{
    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = 0;
    if (is_single_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (is_renderpass_continue)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (is_simultaneous_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CHECK(vkBeginCommandBuffer(pbuffer->handle, &begin_info));
    pbuffer->state = vulkan_command_buffer_state::recording;
}

void end(vulkan_command_buffer* pbuffer)
{
    VK_CHECK(vkEndCommandBuffer(pbuffer->handle));
    pbuffer->state = vulkan_command_buffer_state::recording_ended;
}

void update_submitted(vulkan_command_buffer* pbuffer)
{
    pbuffer->state = vulkan_command_buffer_state::submitted;
}

void reset(vulkan_command_buffer* pbuffer)
{
    pbuffer->state = vulkan_command_buffer_state::ready;
}

void allocate_buffer_begin_single_use(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer)
{
    allocate_buffer(context, pool, true, pbuffer);
    begin(pbuffer, true, false, false);
}

void end_single_use(const vulkan_context& context, VkCommandPool pool, vulkan_command_buffer* pbuffer, VkQueue queue)
{
    // End the command buffer, then submit the queue
    end(pbuffer);

    VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &pbuffer->handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, nullptr));

    // Wait for the queue to finish
    VK_CHECK(vkQueueWaitIdle(queue));

    free_buffer(context, pool, pbuffer);
}
} // namespace sky::graphics::vk::commands
