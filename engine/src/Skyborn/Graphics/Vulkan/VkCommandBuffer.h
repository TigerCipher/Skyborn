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
} // namespace sky::graphics::vk::commands