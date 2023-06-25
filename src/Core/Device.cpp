#include "WackyEngine/Core/Device.h"

#include <cstdint>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iostream>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    const std::vector<const char*> Device::RequiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    QueueFamilyIndices Device::LocateQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices { };

        std::uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (std::size_t i = 0; i < queueFamilyCount; ++i)
        {
            // Graphics Queue Family
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            { 
                indices.GraphicsFamily = i; 
            }

            // Present Queue Family
            VkBool32 presentSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Context::GetWindow()->GetSurface(), &presentSupported);

            if (presentSupported)
            {
                indices.PresentFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }
        }

        return indices;
    }

    Device::Device()
    {
        InitialisePhysicalDevice();
        InitialiseLogicalDevice();
        InitialiseCommandPool();
    }

    Device::~Device()
    {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    void Device::InitialisePhysicalDevice()
    {
        std::uint32_t deviceCount;
        vkEnumeratePhysicalDevices(Context::GetInstance(), &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(Context::GetInstance(), &deviceCount, availableDevices.data());

        // Suitability Checks
        for (std::size_t i = 0; i < deviceCount; ++i)
        {
            QueueFamilyIndices queueFamilies = LocateQueueFamilies(availableDevices[i]);

            // Check 1: Queue Families
            if (!queueFamilies.IsComplete())
            {
                continue;
            }

            // Check 2: Extensions
            std::uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(availableDevices[i], nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(availableDevices[i], nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(RequiredExtensions.begin(), RequiredExtensions.end());

            for (std::size_t j = 0; j < availableExtensions.size(); ++j)
            {
                requiredExtensions.erase(availableExtensions[j].extensionName);
            }

            if (!requiredExtensions.empty())
            {
                continue;
            }

            // Check 3: Swap Chain Support
            std::uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(availableDevices[i], Context::GetWindow()->GetSurface(), &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(availableDevices[i], Context::GetWindow()->GetSurface(), &formatCount, formats.data());

            std::uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(availableDevices[i], Context::GetWindow()->GetSurface(), &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(availableDevices[i], Context::GetWindow()->GetSurface(), &presentModeCount, nullptr);

            if (formats.empty() || presentModes.empty())
            {
                continue;
            }

            // Check 4: Device Features
            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(availableDevices[i], &supportedFeatures);

            if (!supportedFeatures.samplerAnisotropy)
            {
                continue;
            }

            m_PhysicalDevice = availableDevices[i];
            return;
        }

        throw std::runtime_error("Failed to find a suitable graphics device.");
    }

    void Device::InitialiseLogicalDevice()
    {
        QueueFamilyIndices QFIndices = LocateQueueFamilies(m_PhysicalDevice);

        // Queue Creation Information
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo graphicsQueueInfo { };
        graphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueInfo.queueFamilyIndex = QFIndices.GraphicsFamily.value();
        graphicsQueueInfo.queueCount = 1;
        graphicsQueueInfo.pQueuePriorities = &queuePriority;
        queueInfos.push_back(graphicsQueueInfo);

        if (QFIndices.GraphicsFamily != QFIndices.PresentFamily)
        {
            VkDeviceQueueCreateInfo presentQueueInfo { };
            presentQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            presentQueueInfo.queueCount = 1;
            presentQueueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(graphicsQueueInfo);
        }

        // Extra Features to Enabled
        VkPhysicalDeviceFeatures deviceFeatures { };
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        // Logical Device Creation
        VkDeviceCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pQueueCreateInfos = queueInfos.data();
        info.queueCreateInfoCount = static_cast<std::uint32_t>(queueInfos.size());
        info.pEnabledFeatures = &deviceFeatures;
        info.enabledExtensionCount = static_cast<std::uint32_t>(RequiredExtensions.size());
        info.ppEnabledExtensionNames = RequiredExtensions.data();

        if (vkCreateDevice(m_PhysicalDevice, &info, nullptr, &m_LogicalDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create device.");
        }

        // Queue Retrieval
        vkGetDeviceQueue(m_LogicalDevice, QFIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, QFIndices.PresentFamily.value(), 0, &m_PresentQueue);
    }

    void Device::InitialiseCommandPool()
    {
        QueueFamilyIndices QFIndices = LocateQueueFamilies(m_PhysicalDevice);

        // Command Pool Creation
        VkCommandPoolCreateInfo commandPoolInfo { };
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = QFIndices.GraphicsFamily.value();

        if (vkCreateCommandPool(m_LogicalDevice, &commandPoolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    VkSurfaceCapabilitiesKHR Device::GetSurfaceCapabilities() const noexcept
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, Context::GetWindow()->GetSurface(), &capabilities);
        return capabilities;
    }

    VkSurfaceFormatKHR Device::SelectSwapSurfaceFormat() const noexcept
    {
        std::uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, Context::GetWindow()->GetSurface(), &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, Context::GetWindow()->GetSurface(), &formatCount, formats.data());

        for (std::size_t i = 0; i < formatCount; ++i)
        {
            if (formats[i].format == VK_FORMAT_B8G8R8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return formats[i];
            }
        }

        return formats[0];
    }

    VkPresentModeKHR Device::SelectSwapPresentMode() const noexcept
    {
        std::uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, Context::GetWindow()->GetSurface(), &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, Context::GetWindow()->GetSurface(), &presentModeCount, nullptr);

        for (std::size_t i = 0; i < presentModeCount; ++i)
        {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentModes[i];
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Device::SelectSwapExtent() const noexcept
    {
        VkSurfaceCapabilitiesKHR capabilities = GetSurfaceCapabilities();

        if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        
        int width, height;
        glfwGetFramebufferSize(Context::GetWindow()->GetGLFWWindow(), &width, &height);
        VkExtent2D actualExtent = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    std::uint32_t Device::FindMemoryType(std::uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

        for (std::uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
    }

    VkCommandBuffer Device::BeginSingleTimeCommands() const
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo { };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void Device::EndSingleTimeCommands(VkCommandBuffer cmdBuffer) const
    {
        vkEndCommandBuffer(cmdBuffer);

        VkSubmitInfo submitInfo { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_GraphicsQueue);

        vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &cmdBuffer);
    }

    void Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const
    {
        VkBufferCreateInfo bufferInfo { };
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer.");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer memory.");
        }

        vkBindBufferMemory(m_LogicalDevice, buffer, memory, 0);
    }

    void Device::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
    {
        VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion { };
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(cmdBuffer);
    }

    void Device::CreateImage(std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const
    {
        VkImageCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.format = format;
        info.tiling = tiling;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = usage;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.flags = 0;

        if (vkCreateImage(m_LogicalDevice, &info, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image.");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(m_LogicalDevice, image, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image memory.");
        }

        vkBindImageMemory(m_LogicalDevice, image, imageMemory, 0);
    }

    void Device::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const
    {
        VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier { };
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } 
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } 
        else 
        {
            throw std::invalid_argument("Unsupported layout transition.");
        }

        vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        EndSingleTimeCommands(cmdBuffer);
    }

    void Device::CopyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height) const
    {
        VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();

        VkBufferImageCopy region { };
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        EndSingleTimeCommands(cmdBuffer);
    }
}