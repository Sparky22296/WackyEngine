#include "WackyEngine/Graphics/SwapChain.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    const int SwapChain::MAX_FRAMES_IN_FLIGHT = 3;

    SwapChain::SwapChain()
    {
        Initialise();
    }

    void SwapChain::Initialise()
    {
        CreateSwapchain();
        InitialiseSyncObjects();
    }

    void SwapChain::Reinitialise()
    {
        while (Context::GetWindow()->GetWidth() == 0 || Context::GetWindow()->GetHeight() == 0)
        {
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(Context::GetDevice()->GetLogicalDevice());

        // Cleaning Old Objects

        delete m_RenderPass;

        for (std::size_t i = 0; i < m_Framebuffers.size(); ++i)
        {
            vkDestroyFramebuffer(Context::GetDevice()->GetLogicalDevice(), m_Framebuffers[i], nullptr);
        }

        for (std::size_t i = 0; i < m_ImageViews.size(); ++i)
        {
            vkDestroyImageView(Context::GetDevice()->GetLogicalDevice(), m_ImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(Context::GetDevice()->GetLogicalDevice(), m_SwapChain, nullptr);

        // Recreating Swap Chain

        CreateSwapchain();
    }

    void SwapChain::CreateSwapchain()
    {
        m_CurrentFrame = 0;

        VkSurfaceCapabilitiesKHR capabilities = Context::GetDevice()->GetSurfaceCapabilities();
        m_Format = Context::GetDevice()->SelectSwapSurfaceFormat();
        m_PresentMode = Context::GetDevice()->SelectSwapPresentMode();
        m_Extent = Context::GetDevice()->SelectSwapExtent();

        m_RenderPass = RenderPass::CreateSimplePass(m_Format.format);

        VkSwapchainCreateInfoKHR info { };
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.minImageCount = std::clamp(capabilities.minImageCount + 1, (std::uint32_t)0, capabilities.maxImageCount);;
        info.imageFormat = m_Format.format;
        info.imageColorSpace = m_Format.colorSpace;
        info.imageExtent = m_Extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = Device::LocateQueueFamilies(Context::GetDevice()->GetPhysicalDevice());
        std::uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        if (indices.GraphicsFamily.value() == indices.PresentFamily.value())
        {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount = 0;
            info.pQueueFamilyIndices = nullptr;
        }
        else
        {
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        }

        info.preTransform = capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.clipped = VK_TRUE;
        info.surface = Context::GetWindow()->GetSurface();
        info.presentMode = m_PresentMode;
        info.oldSwapchain = nullptr;

        if (vkCreateSwapchainKHR(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &m_SwapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swap chain.");
        }

        InitialiseImageViews();
        InitialiseFramebuffers();
    }

    void SwapChain::InitialiseImageViews()
    {
        // Image Retrieval

        std::uint32_t imageCount;
        vkGetSwapchainImagesKHR(Context::GetDevice()->GetLogicalDevice(), m_SwapChain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(Context::GetDevice()->GetLogicalDevice(), m_SwapChain, &imageCount, m_Images.data());

        // Image View Creation

        m_ImageViews.resize(imageCount);

        for (std::size_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo viewInfo { };
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_Images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_Format.format;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(Context::GetDevice()->GetLogicalDevice(), &viewInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image view.");
            }
        }
    }

    void SwapChain::InitialiseSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderCompleteSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo { };
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo { };
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (vkCreateSemaphore(Context::GetDevice()->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(Context::GetDevice()->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_RenderCompleteSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(Context::GetDevice()->GetLogicalDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create syncronisation objects.");
            }
        }
    }
    
    void SwapChain::InitialiseFramebuffers()
    {
        m_Framebuffers.resize(m_ImageViews.size());

        for (std::size_t i = 0; i < m_ImageViews.size(); ++i)
        {
            VkImageView attachments[] = { m_ImageViews[i] };

            VkFramebufferCreateInfo info { };
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.renderPass = m_RenderPass->GetRenderPass();
            info.attachmentCount = 1;
            info.pAttachments = attachments;
            info.width = m_Extent.width;
            info.height = m_Extent.height;
            info.layers = 1;

            if (vkCreateFramebuffer(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &(m_Framebuffers[i])) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer.");
            }
        }
    }

    SwapChain::~SwapChain()
    {
        delete m_RenderPass;

        for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(Context::GetDevice()->GetLogicalDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(Context::GetDevice()->GetLogicalDevice(), m_RenderCompleteSemaphores[i], nullptr);
            vkDestroyFence(Context::GetDevice()->GetLogicalDevice(), m_InFlightFences[i], nullptr);
        }

        for (std::size_t i = 0; i < m_Framebuffers.size(); ++i)
        {
            vkDestroyFramebuffer(Context::GetDevice()->GetLogicalDevice(), m_Framebuffers[i], nullptr);
        }

        for (std::size_t i = 0; i < m_ImageViews.size(); ++i)
        {
            vkDestroyImageView(Context::GetDevice()->GetLogicalDevice(), m_ImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(Context::GetDevice()->GetLogicalDevice(), m_SwapChain, nullptr);
    }

    VkResult SwapChain::AcquireNextImage(std::uint32_t& imageIndex)
    {
        vkWaitForFences(Context::GetDevice()->GetLogicalDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        return vkAcquireNextImageKHR(Context::GetDevice()->GetLogicalDevice(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
    }

    VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer buffer, const std::uint32_t imageIndex)
    {
        vkResetFences(Context::GetDevice()->GetLogicalDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { m_RenderCompleteSemaphores[m_CurrentFrame] };
        VkSubmitInfo submitInfo { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(Context::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer.");
        }

        VkSwapchainKHR swapChains[] = { m_SwapChain };
        VkPresentInfoKHR presentInfo { };
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR(Context::GetDevice()->GetPresentQueue(), &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }
}