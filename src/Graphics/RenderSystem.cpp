#include "WackyEngine/Graphics/RenderSystem.h"

#include <stdexcept>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    RenderSystem::RenderSystem()
    {
        m_ClearColour = { 0.2f, 0.2f, 0.2f };

        m_SwapChain = new SwapChain();
        InitialiseCommandBuffers();
    }

    RenderSystem::~RenderSystem()
    {
        vkFreeCommandBuffers(Context::GetDevice()->GetLogicalDevice(), Context::GetDevice()->GetCommandPool(), (std::uint32_t)m_CommandBuffers.size(), m_CommandBuffers.data());
        delete m_SwapChain;
    }

    void RenderSystem::InitialiseCommandBuffers()
    {
        VkCommandPool pool = Context::GetDevice()->GetCommandPool();

        m_CommandBuffers.resize(m_SwapChain->MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo commandBufferInfo { };
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = pool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = (std::uint32_t)m_SwapChain->MAX_FRAMES_IN_FLIGHT;

        if (vkAllocateCommandBuffers(Context::GetDevice()->GetLogicalDevice(), &commandBufferInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command buffers.");
        }
    }

    VkCommandBuffer RenderSystem::BeginFrame()
    {
        VkResult result = m_SwapChain->AcquireNextImage(m_CurrentIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_SwapChain->Reinitialise();
            return nullptr;
        }
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image.");
        }

        m_FrameStarted = true;

        // Command Buffer Begin

        VkCommandBuffer buffer = m_CommandBuffers[m_CurrentIndex];
        
        VkCommandBufferBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(buffer, &info) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording to command buffer.");
        }

        return buffer;
    }

    void RenderSystem::EndFrame()
    {
        VkCommandBuffer buffer = m_CommandBuffers[m_CurrentIndex];
        if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record to command buffer.");
        }

        VkResult result = m_SwapChain->SubmitCommandBuffers(m_CommandBuffers[m_CurrentIndex], m_CurrentIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_SwapChain->Reinitialise();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to present swap chain image.");
        }

        m_FrameStarted = false;
    }

    void RenderSystem::BeginRenderPass(VkCommandBuffer buffer)
    {
        VkRenderPassBeginInfo renderBeginInfo { };
        renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginInfo.renderPass = GetSwapRenderPass()->GetRenderPass();
        renderBeginInfo.framebuffer = m_SwapChain->GetFramebuffers()[m_CurrentIndex];
        renderBeginInfo.renderArea.offset = { 0, 0 };
        renderBeginInfo.renderArea.extent = m_SwapChain->GetExtent();

        VkClearValue clearColour = {{{ m_ClearColour.X, m_ClearColour.Y, m_ClearColour.Z, 1.0f }}};
        renderBeginInfo.clearValueCount = 1;
        renderBeginInfo.pClearValues = &clearColour;

        vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentIndex], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport { };
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->GetExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffers[m_CurrentIndex], 0, 1, &viewport);

        VkRect2D scissor { };
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChain->GetExtent();
        vkCmdSetScissor(m_CommandBuffers[m_CurrentIndex], 0, 1, &scissor);
    }

    void RenderSystem::EndRenderPass(VkCommandBuffer buffer)
    {
        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentIndex]);
    }
}