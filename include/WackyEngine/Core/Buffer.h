#ifndef WACKYENGINE_CORE_BUFFER_H_
#define WACKYENGINE_CORE_BUFFER_H_

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Device.h"

namespace WackyEngine
{
    class Buffer
    {
    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;

    public:
        Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();

        void SetData(void* data, const std::size_t size);
        void CopyBuffer(Buffer& buffer, const std::size_t size);

        inline VkBuffer GetBufferObject() const noexcept { return m_Buffer; }
        inline VkDeviceMemory GetMemoryObject() const noexcept { return m_Memory; }
    };
}

#endif