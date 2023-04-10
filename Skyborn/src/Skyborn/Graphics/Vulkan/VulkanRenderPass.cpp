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
//  File Name: VulkanRenderPass.cpp
//  Date File Created: 04/02/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "VulkanRenderPass.h"

namespace sky::graphics::vk::renderpass
{
void create(vulkan_context* context, vulkan_renderpass* prenderpass, f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a,
            f32 depth, f32 stencil)
{
    prenderpass->x       = x;
    prenderpass->y       = y;
    prenderpass->w       = w;
    prenderpass->h       = h;

    prenderpass->r       = r;
    prenderpass->g       = g;
    prenderpass->b       = b;
    prenderpass->a       = a;

    prenderpass->depth   = depth;
    prenderpass->stencil = (u32)stencil;

    // Main subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments
    constexpr u32           attachments_desc_count{ 2 };
    VkAttachmentDescription attachment_descs[attachments_desc_count];

    // Color attachment
    VkAttachmentDescription color_attachment{};
    color_attachment.format         = context->swapchain.image_format.format;
    color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags          = 0;

    attachment_descs[0] = color_attachment;

    // Color attachment reference
    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &color_attachment_ref;

    // Depth attachment
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format         = context->device.depth_format;
    depth_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment_descs[1] = depth_attachment;

    // Depth attachment reference
    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Depth stencil data
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    // TODO: Additional attachments

    // Shader input
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments    = nullptr;

    // Attachments used for multisampling color attachments
    subpass.pResolveAttachments = nullptr;

    // Attachments not used in this subpass, but must be preserved for the next.
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments    = nullptr;

    // Render pass dependencies
    VkSubpassDependency dependency{};
    dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass      = 0;
    dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask   = 0;
    dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    // Render pass create
    VkRenderPassCreateInfo rpass_create_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    rpass_create_info.attachmentCount = attachments_desc_count;
    rpass_create_info.pAttachments    = attachment_descs;
    rpass_create_info.subpassCount    = 1;
    rpass_create_info.pSubpasses      = &subpass;
    rpass_create_info.dependencyCount = 1;
    rpass_create_info.pDependencies   = &dependency;
    rpass_create_info.pNext           = nullptr;
    rpass_create_info.flags           = 0;

    VK_CHECK(vkCreateRenderPass(context->device.logical, &rpass_create_info, context->allocator, &prenderpass->handle));
}

void destroy(const vulkan_context& context, vulkan_renderpass* prenderpass)
{
    if (prenderpass && prenderpass->handle)
    {
        vkDestroyRenderPass(context.device.logical, prenderpass->handle, context.allocator);
        prenderpass->handle = nullptr;
    }
}

void begin(vulkan_command_buffer* cmd_buffer, const vulkan_renderpass& renderpass, VkFramebuffer frame_buffer)
{
    VkRenderPassBeginInfo begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    begin_info.renderPass               = renderpass.handle;
    begin_info.framebuffer              = frame_buffer;
    begin_info.renderArea.offset.x      = (i32) renderpass.x;
    begin_info.renderArea.offset.y      = (i32) renderpass.y;
    begin_info.renderArea.extent.width  = (i32) renderpass.w;
    begin_info.renderArea.extent.height = (i32) renderpass.h;

    VkClearValue clear_values[2]{};
    clear_values[0].color.float32[0]     = renderpass.r;
    clear_values[0].color.float32[1]     = renderpass.g;
    clear_values[0].color.float32[2]     = renderpass.b;
    clear_values[0].color.float32[3]     = renderpass.a;
    clear_values[1].depthStencil.depth   = renderpass.depth;
    clear_values[1].depthStencil.stencil = renderpass.stencil;

    begin_info.clearValueCount = 2;
    begin_info.pClearValues    = clear_values;

    vkCmdBeginRenderPass(cmd_buffer->handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    cmd_buffer->state = vulkan_command_buffer_state::in_render_pass;
}

void end(vulkan_command_buffer* cmd_buffer, [[maybe_unused]] vulkan_renderpass* prenderpass)
{
    vkCmdEndRenderPass(cmd_buffer->handle);
    cmd_buffer->state = vulkan_command_buffer_state::recording;
}


} // namespace sky::graphics::vk::renderpass