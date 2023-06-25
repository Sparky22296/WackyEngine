#include "WackyEngine/Core/Buffer.h"

#include <stdexcept>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VkBufferCreateInfo bufferInfo { };
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(Context::GetDevice()->GetLogicalDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer.");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(Context::GetDevice()->GetLogicalDevice(), m_Buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = Context::GetDevice()->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(Context::GetDevice()->GetLogicalDevice(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer memory.");
        }

        vkBindBufferMemory(Context::GetDevice()->GetLogicalDevice(), m_Buffer, m_Memory, 0);
    }

    Buffer::~Buffer()
    {
        vkDestroyBuffer(Context::GetDevice()->GetLogicalDevice(), m_Buffer, nullptr);
        vkFreeMemory(Context::GetDevice()->GetLogicalDevice(), m_Memory, nullptr);
    }

    void Buffer::SetData(void* data, const std::size_t size)
    {
        void* stage;
        vkMapMemory(Context::GetDevice()->GetLogicalDevice(), m_Memory, 0, size, 0, &stage);
        memcpy(stage, data, size);
        vkUnmapMemory(Context::GetDevice()->GetLogicalDevice(), m_Memory);
    }

    void Buffer::CopyBuffer(Buffer& buffer, const std::size_t size)
    {
        VkCommandBuffer cmdBuffer = Context::GetDevice()->BeginSingleTimeCommands();

        VkBufferCopy copyRegion { };
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(cmdBuffer, buffer.GetBufferObject(), m_Buffer, 1, &copyRegion);

        Context::GetDevice()->EndSingleTimeCommands(cmdBuffer);
    }
}