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
// File Name: VkFramebuffer.cpp
// Date File Created: 04/23/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#include "VkFramebuffer.h"
#include "VkCore.h"

namespace sky::graphics::vk::framebuffer
{
vk_framebuffer create(vk_renderpass& renderpass, u32 width, u32 height, const utl::heap_array<VkImageView>& attachments)
{
    vk_framebuffer buffer{};

    buffer.attachments.initialize(attachments.size());

    for (u32 i = 0; i < attachments.size(); ++i)
    {
        buffer.attachments[i] = attachments[i];
    }

    buffer.renderpass = &renderpass;
    VkFramebufferCreateInfo fb_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    fb_info.renderPass      = renderpass.handle;
    fb_info.attachmentCount = (u32) attachments.size();
    fb_info.pAttachments    = buffer.attachments.data();
    fb_info.width           = width;
    fb_info.height          = height;
    fb_info.layers          = 1;

    VK_CALL(vkCreateFramebuffer(core::logical_device(), &fb_info, core::allocator(), &buffer.handle));

    return buffer;
}

void destroy(vk_framebuffer& buffer)
{
    vkDestroyFramebuffer(core::logical_device(), buffer.handle, core::allocator());
    buffer.attachments.clear();
    buffer.handle     = nullptr;
    buffer.renderpass = nullptr;
}
} // namespace sky::graphics::vk::framebuffer