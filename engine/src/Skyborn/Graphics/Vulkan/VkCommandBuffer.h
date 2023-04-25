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
// File Name: VkCommandBuffer.h
// Date File Created: 04/23/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "VkCommon.h"

namespace sky::graphics::vk
{
class vk_surface;
}

namespace sky::graphics::vk::commands
{
vk_command_buffer allocate_buffer(VkCommandPool pool, bool is_primary);
void              free_buffer(VkCommandPool pool, vk_command_buffer& buffer);
void begin(vk_command_buffer& buffer, bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use);
void end(vk_command_buffer& buffer);
void update_submitted(vk_command_buffer& buffer);
void reset(vk_command_buffer& buffer);
vk_command_buffer allocate_buffer_begin_single_use(VkCommandPool pool, vk_command_buffer& buffer);
void              end_single_use(VkCommandPool pool, vk_command_buffer& buffer, VkQueue queue);

class vk_command
{
public:
    vk_command() = default;
    DISABLE_COPY_AND_MOVE(vk_command);

    explicit vk_command(const vk_surface& surface, u32 queue_family_index);

    void free_command_buffer(u32 index);
    void create_command_buffers();
    void nullify_inflight_images();

    bool begin_frame(vk_surface& surface, f32 delta);
    bool end_frame(vk_surface& surface, f32 delta);

    void destroy();

    [[nodiscard]] constexpr VkCommandPool pool() const { return m_pool; }

private:
    VkCommandPool                  m_pool{};
    VkQueue                        m_graphics_queue{};
    VkQueue                        m_present_queue{};
    utl::vector<vk_command_buffer> m_cmd_buffers{};
    utl::vector<VkSemaphore>       m_image_available_semaphores{};
    utl::vector<VkSemaphore>       m_queue_complete_semaphores{};
    utl::vector<vk_fence>          m_inflight_fences{};
    utl::vector<vk_fence*>         m_images_in_flight{};
    u32                            m_swapchain_image_count{};
    u8                             m_max_frames_in_flight{};
};
} // namespace sky::graphics::vk::commands