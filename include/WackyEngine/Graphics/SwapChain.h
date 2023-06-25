#ifndef WACKYENGINE_GRAPHICS_SWAPCHAIN_H_
#define WACKYENGINE_GRAPHICS_SWAPCHAIN_H_

#include <vector>

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Window.h"
#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Graphics/RenderPass.h"

namespace WackyEngine
{
    class SwapChain
    {
    private:
        VkSwapchainKHR m_SwapChain;

        std::uint32_t m_CurrentFrame = 0;

        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        std::vector<VkFramebuffer> m_Framebuffers;

        RenderPass* m_RenderPass;

        VkSurfaceFormatKHR m_Format;
        VkPresentModeKHR m_PresentMode;
        VkExtent2D m_Extent;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderCompleteSemaphores;
        std::vector<VkFence> m_InFlightFences;

        void CreateSwapchain();

        void InitialiseImageViews();
        void InitialiseFramebuffers();
        void InitialiseSyncObjects();

    public:
        static const int MAX_FRAMES_IN_FLIGHT;

        SwapChain();
        ~SwapChain();

        void Initialise();
        void Reinitialise();

        VkResult AcquireNextImage(std::uint32_t& imageIndex);
        VkResult SubmitCommandBuffers(const VkCommandBuffer buffer, const std::uint32_t imageIndex);

        inline VkSwapchainKHR GetSwapchainObject() const noexcept { return m_SwapChain; }
        inline std::vector<VkImage> GetImages() const noexcept { return m_Images; }
        inline std::vector<VkImageView> GetImageViews() const noexcept { return m_ImageViews; }
        inline VkSurfaceFormatKHR GetFormat() const noexcept { return m_Format; }
        inline VkPresentModeKHR GetPresentMode() const noexcept { return m_PresentMode; }
        inline VkExtent2D GetExtent() const noexcept { return m_Extent; }
        inline std::vector<VkFramebuffer> GetFramebuffers() const noexcept { return m_Framebuffers; }
        inline RenderPass* GetRenderPass() const noexcept { return m_RenderPass; }
    };
}

#endif