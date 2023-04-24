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
// File Name: VkCommandBuffer.cpp
// Date File Created: 04/23/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "VkCommandBuffer.h"
#include "VkCore.h"

namespace sky::graphics::vk::commands
{
vk_command_buffer allocate_buffer(VkCommandPool pool, bool is_primary)
{
    vk_command_buffer           buffer{};
    VkCommandBufferAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    alloc_info.commandPool        = pool;
    alloc_info.level              = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    alloc_info.commandBufferCount = 1;
    alloc_info.pNext              = nullptr;

    buffer.current_state = vk_command_buffer::not_allocated;
    VK_CALL(vkAllocateCommandBuffers(core::logical_device(), &alloc_info, &buffer.handle));
    buffer.current_state = vk_command_buffer::ready;

    return buffer;
}

void free_buffer(VkCommandPool pool, vk_command_buffer& buffer)
{
    vkFreeCommandBuffers(core::logical_device(), pool, 1, &buffer.handle);
    buffer.handle        = nullptr;
    buffer.current_state = vk_command_buffer::not_allocated;
}

void begin(vk_command_buffer& buffer, bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use)
{
    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = 0;
    if (is_single_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (is_renderpass_continue)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (is_simultaneous_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL(vkBeginCommandBuffer(buffer.handle, &begin_info));
    buffer.current_state = vk_command_buffer::recording;
}

void end(vk_command_buffer& buffer)
{
    VK_CALL(vkEndCommandBuffer(buffer.handle));
    buffer.current_state = vk_command_buffer::recording_ended;
}

void update_submitted(vk_command_buffer& buffer)
{
    buffer.current_state = vk_command_buffer::submitted;
}

void reset(vk_command_buffer& buffer)
{
    buffer.current_state = vk_command_buffer::ready;
}

vk_command_buffer allocate_buffer_begin_single_use(VkCommandPool pool)
{
    vk_command_buffer buffer = allocate_buffer(pool, true);
    begin(buffer, true, false, false);
    return buffer;
}

void end_single_use(VkCommandPool pool, vk_command_buffer& buffer, VkQueue queue)
{
    end(buffer);
    VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &buffer.handle;
    VK_CALL(vkQueueSubmit(queue, 1, &submit_info, nullptr));
    // Wait for the queue to finish
    VK_CALL(vkQueueWaitIdle(queue));

    free_buffer(pool, buffer);
}
} // namespace sky::graphics::vk::commands