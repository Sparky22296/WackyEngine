#ifndef WACKYENGINE_CORE_DEVICE_H_
#define WACKYENGINE_CORE_DEVICE_H_

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Window.h"

namespace WackyEngine
{
    struct QueueFamilyIndices
    {
        std::optional<std::uint32_t> GraphicsFamily;
        std::optional<std::uint32_t> PresentFamily;

        inline bool IsComplete() { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
    };

    class Device
    {
    private:
        static const std::vector<const char*> RequiredExtensions;

        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_LogicalDevice;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkCommandPool m_CommandPool;

        void InitialisePhysicalDevice();
        void InitialiseLogicalDevice();
        void InitialiseCommandPool();

    public:
        static QueueFamilyIndices LocateQueueFamilies(VkPhysicalDevice device);

        Device();
        ~Device();

        void Initialise();
        
        inline VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }
        inline VkDevice GetLogicalDevice() const noexcept { return m_LogicalDevice; }
        inline VkQueue GetGraphicsQueue() const noexcept { return m_GraphicsQueue; }
        inline VkQueue GetPresentQueue() const noexcept { return m_PresentQueue; }
        inline VkCommandPool GetCommandPool() const noexcept { return m_CommandPool; }

        std::uint32_t FindMemoryType(std::uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const noexcept;
        VkSurfaceFormatKHR SelectSwapSurfaceFormat() const noexcept;
        VkPresentModeKHR SelectSwapPresentMode() const noexcept;
        VkExtent2D SelectSwapExtent() const noexcept;

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void CreateImage(std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
        void CopyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height) const;

        VkCommandBuffer BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(VkCommandBuffer cmdBuffer) const;
    };
}

#endif