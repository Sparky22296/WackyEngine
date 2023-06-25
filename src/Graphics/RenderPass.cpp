#include "WackyEngine/Graphics/RenderPass.h"

#include <stdexcept>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    RenderPass* RenderPass::CreateSimplePass(VkFormat format)
    {
        VkAttachmentDescription colourAttachment { };
        colourAttachment.format = format;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colourAttachmentRef { };
        colourAttachmentRef.attachment = 0;
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass { };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colourAttachmentRef;

        VkRenderPassCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &colourAttachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;

        VkSubpassDependency dependency { };
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        info.dependencyCount = 1;
        info.pDependencies = &dependency;

        RenderPass* renderPass = new RenderPass();

        if (vkCreateRenderPass(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &renderPass->m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass.");
        }

        return renderPass;
    }

    RenderPass::~RenderPass()
    {
        vkDestroyRenderPass(Context::GetDevice()->GetLogicalDevice(), m_RenderPass, nullptr);
    }
}