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
//  File Name: VulkanFramebuffer.cpp
//  Date File Created: 04/04/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanFramebuffer.h"

namespace sky::graphics::vk::framebuffer
{
void create(const vulkan_context& context, vulkan_renderpass* renderpass, u32 width, u32 height, u32 attachment_count,
            const VkImageView* attachments, vulkan_framebuffer* pbuffer)
{
    memory::allocate(pbuffer->attachments, memory_tag::renderer, attachment_count);
    pbuffer->attachment_count = attachment_count;
    for (u32 i = 0; i < attachment_count; ++i)
    {
        pbuffer->attachments[i] = attachments[i];
    }

    pbuffer->renderpass = renderpass;

    VkFramebufferCreateInfo fb_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    fb_info.renderPass      = renderpass->handle;
    fb_info.attachmentCount = attachment_count;
    fb_info.pAttachments    = pbuffer->attachments;
    fb_info.width           = width;
    fb_info.height          = height;
    fb_info.layers          = 1;

    VK_CHECK(vkCreateFramebuffer(context.device.logical, &fb_info, context.allocator, &pbuffer->handle));
}

void destroy(const vulkan_context& context, vulkan_framebuffer* pbuffer)
{
    vkDestroyFramebuffer(context.device.logical, pbuffer->handle, context.allocator);
    memory::free_(pbuffer->attachments, memory_tag::renderer, pbuffer->attachment_count);
    pbuffer->handle           = nullptr;
    pbuffer->renderpass       = nullptr;
    pbuffer->attachment_count = 0;
}

} // namespace sky::graphics::vk::framebuffer
