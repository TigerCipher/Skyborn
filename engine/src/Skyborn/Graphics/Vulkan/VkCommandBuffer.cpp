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
#include "VkFence.h"
#include "VkRenderpass.h"
#include "VkSurface.h"

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

vk_command::vk_command(const vk_surface& surface, u32 queue_family_index)
{
    m_swapchain_image_count = surface.swapchain().images().size();


    VkCommandPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL(vkCreateCommandPool(core::logical_device(), &pool_info, core::allocator(), &m_pool));

    m_graphics_queue = core::device().graphics_queue;
    m_present_queue  = core::device().present_queue;

    create_command_buffers();

    m_max_frames_in_flight = surface.swapchain().max_frames_in_flight();
    m_image_available_semaphores.resize(m_max_frames_in_flight);
    m_queue_complete_semaphores.resize(m_max_frames_in_flight);
    m_inflight_fences.resize(m_max_frames_in_flight);

    for (u32 i = 0; i < m_max_frames_in_flight; ++i)
    {
        VkSemaphoreCreateInfo sem_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(core::logical_device(), &sem_info, core::allocator(), &m_image_available_semaphores[i]);
        vkCreateSemaphore(core::logical_device(), &sem_info, core::allocator(), &m_queue_complete_semaphores[i]);
        m_inflight_fences[i] = fence::create(true);
    }

    m_images_in_flight.resize(m_swapchain_image_count, VK_NULL_HANDLE);
}

void vk_command::free_command_buffer(u32 index)
{
    free_buffer(m_pool, m_cmd_buffers[index]);
}

void vk_command::create_command_buffers()
{
    if(m_cmd_buffers.empty())
    {
        m_cmd_buffers.resize(m_swapchain_image_count);
    }

    for(auto& cb : m_cmd_buffers)
    {
        if(cb.handle)
        {
            free_buffer(m_pool, cb);
        }
        cb = allocate_buffer(m_pool, true);
    }

    LOG_INFO("Vulkan command buffers created");
}

void vk_command::nullify_inflight_images()
{
    for (auto& iff : m_images_in_flight)
    {
        iff = VK_NULL_HANDLE;
    }
}

bool vk_command::begin_frame(vk_surface& surface, f32 delta)
{
    const VkDevice         logical = core::logical_device();

    if (surface.recreating())
    {
        const VkResult res = vkDeviceWaitIdle(logical);
        if (!was_success(res))
        {
            LOG_ERROR("Vulkan vkDeviceWaitIdle in begin_frame failed - {}", get_result_string(res, true));
            return false;
        }
        LOG_DEBUG("Recreating swapchain...");
        return false;
    }

    if (surface::was_framebuffer_resized())
    {
        const VkResult res = vkDeviceWaitIdle(logical);
        if (!was_success(res))
        {
            LOG_ERROR("Vulkan vkDeviceWaitIdle in begin_frame failed - {}", get_result_string(res, true));
            return false;
        }

        if (!surface.recreate_swapchain(this))
        {
            return false;
        }

        LOG_DEBUG("Resized...");
        return false;
    }

    const u32 frame = surface.frame_index();

    if (!fence::wait(m_inflight_fences[frame], u64_max))
    {
        LOG_ERROR("Inflight fence wait failed");
        return false;
    }

    if (!surface.swapchain_acquire_next_image(u64_max, m_image_available_semaphores[frame], nullptr))
    {
        return false;
    }

    const u32 img   = surface.image_index();
    vk_command_buffer& cmd_buffer = m_cmd_buffers[img];
    reset(cmd_buffer);
    begin(cmd_buffer, false, false, false);

    VkViewport viewport{};
    const u32  fb_width  = surface.width();
    const u32  fb_height = surface.height();
    viewport.x           = 0.0f;
    viewport.y           = (f32) fb_height;
    viewport.width       = (f32) fb_width;
    viewport.height      = -(f32) fb_height;
    viewport.minDepth    = 0.0f;
    viewport.maxDepth    = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width                = fb_width;
    scissor.extent.height               = fb_height;

    vkCmdSetViewport(cmd_buffer.handle, 0, 1, &viewport);
    vkCmdSetScissor(cmd_buffer.handle, 0, 1, &scissor);

    surface.renderpass().render_area.z = (f32) fb_width;
    surface.renderpass().render_area.w = (f32) fb_height;
    surface.begin_renderpass(cmd_buffer);
    return true;
}

bool vk_command::end_frame(vk_surface& surface, f32 delta)
{
    const u32 frame = surface.frame_index();
    const u32 img   = surface.image_index();
    vk_command_buffer& cmd_buffer = m_cmd_buffers[img];
    renderpass::end(surface.renderpass(), cmd_buffer);
    end(cmd_buffer);

    if (m_images_in_flight[img] != VK_NULL_HANDLE)
    {
        fence::wait(*m_images_in_flight[img], u64_max);
    }

    m_images_in_flight[img] = &m_inflight_fences[frame];

    fence::reset(m_inflight_fences[frame]);

    VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = &cmd_buffer.handle;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = &m_queue_complete_semaphores[frame];
    submit_info.waitSemaphoreCount   = 1;
    submit_info.pWaitSemaphores      = &m_image_available_semaphores[frame];

    constexpr VkPipelineStageFlags flags{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.pWaitDstStageMask = &flags;

    const VkResult res = vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_inflight_fences[frame].handle);
    if (res != VK_SUCCESS)
    {
        LOG_ERROR("vkQueueSubmit failed - {}", get_result_string(res, true));
        return false;
    }

    update_submitted(cmd_buffer);
    surface.present(m_graphics_queue, m_present_queue, m_queue_complete_semaphores[frame]);
    return true;
}

void vk_command::destroy()
{
    for (u8 i = 0; i < m_max_frames_in_flight; ++i)
    {
        if (m_image_available_semaphores[i])
        {
            vkDestroySemaphore(core::logical_device(), m_image_available_semaphores[i], core::allocator());
        }

        if (m_queue_complete_semaphores[i])
        {
            vkDestroySemaphore(core::logical_device(), m_queue_complete_semaphores[i], core::allocator());
        }

        fence::destroy(m_inflight_fences[i]);
    }
    m_image_available_semaphores.clear();
    m_queue_complete_semaphores.clear();
    m_inflight_fences.clear();

    for (auto& cb : m_cmd_buffers)
    {
        if (cb.handle)
        {
            commands::free_buffer(m_pool, cb);
            cb.handle = nullptr;
        }
    }

    m_cmd_buffers.clear();

    LOG_INFO("Destroying command pools");
    vkDestroyCommandPool(core::logical_device(), m_pool, core::allocator());
}
} // namespace sky::graphics::vk::commands