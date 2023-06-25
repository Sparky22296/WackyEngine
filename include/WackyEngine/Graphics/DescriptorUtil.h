#ifndef WACKYENGINE_GRAPHICS_DESCRIPTORUTIL_H_
#define WACKYENGINE_GRAPHICS_DESCRIPTORUTIL_H_

#include <cstdint>

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Device.h"

namespace WackyEngine
{
    class DescriptorPoolBuilder
    {
    private:
        std::uint32_t m_MaxSets;
        std::vector<VkDescriptorPoolSize> m_PoolSizes;

    public:
        DescriptorPoolBuilder(std::uint32_t maxSets) : m_MaxSets(maxSets) { }

        DescriptorPoolBuilder& AddPoolSize(VkDescriptorType type, std::uint32_t count);
        VkDescriptorPool Build();
    };

    class DescriptorSetLayoutBuilder
    {
    private:
        std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

    public:
        DescriptorSetLayoutBuilder() { }

        DescriptorSetLayoutBuilder& AddBinding(std::uint32_t binding, VkDescriptorType type, std::uint32_t count, VkShaderStageFlags flags, VkSampler* immutableSamplers = nullptr);
        VkDescriptorSetLayout Build();
    };

    class DescriptorWriter
    {
    private:
        VkDescriptorSet m_DestinationSet;
        std::vector<VkWriteDescriptorSet> m_DescriptorWrites;

    public:
        DescriptorWriter(VkDescriptorSet destinationSet) : m_DestinationSet(destinationSet) { }

        DescriptorWriter& WriteBuffer(std::uint32_t binding, VkDescriptorType type, VkDescriptorBufferInfo* bufferInfo);
        DescriptorWriter& WriteImage(std::uint32_t binding, VkDescriptorType type, VkDescriptorImageInfo* imageInfo);

        void Write();
    };
}

#endif