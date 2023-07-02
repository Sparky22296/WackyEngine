#include "WackyEngine/Graphics/DescriptorUtil.h"

#include <stdexcept>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    // Descriptor Pool

    DescriptorPoolBuilder& DescriptorPoolBuilder::AddPoolSize(VkDescriptorType type, std::uint32_t count)
    {
        VkDescriptorPoolSize size { };
        size.type = type;
        size.descriptorCount = count;

        m_PoolSizes.push_back(size);

        return *this;
    }
    
    VkDescriptorPool DescriptorPoolBuilder::Build()
    {
        VkDescriptorPoolCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.poolSizeCount = (std::uint32_t)m_PoolSizes.size();
        info.pPoolSizes = m_PoolSizes.data();
        info.maxSets = m_MaxSets;

        VkDescriptorPool pool;

        if (vkCreateDescriptorPool(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool.");
        }

        return pool;
    }

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::AddBinding(std::uint32_t binding, VkDescriptorType type, std::uint32_t count, VkShaderStageFlags flags, VkSampler* immutableSamplers)
    {
        VkDescriptorSetLayoutBinding layoutBinding { };
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = type;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = flags;
        layoutBinding.pImmutableSamplers = immutableSamplers;

        m_Bindings.push_back(layoutBinding);

        return *this;
    }

    VkDescriptorSetLayout DescriptorSetLayoutBuilder::Build()
    {
        VkDescriptorSetLayoutCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = (std::uint32_t)m_Bindings.size();
        info.pBindings = m_Bindings.data();

        VkDescriptorSetLayout layout;

        if (vkCreateDescriptorSetLayout(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }

        return layout;
    }

    // Descriptor Writer

    DescriptorWriter& DescriptorWriter::WriteBuffer(std::uint32_t binding, std::uint32_t count, VkDescriptorType type, VkDescriptorBufferInfo* bufferInfo)
    {
        VkWriteDescriptorSet write { };
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DestinationSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = type;
        write.descriptorCount = count;
        write.pBufferInfo = bufferInfo;

        m_DescriptorWrites.push_back(write);

        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(std::uint32_t binding, std::uint32_t count, VkDescriptorType type, VkDescriptorImageInfo* imageInfo)
    {
        VkWriteDescriptorSet write { };
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DestinationSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = type;
        write.descriptorCount = count;
        write.pImageInfo = imageInfo;

        m_DescriptorWrites.push_back(write);

        return *this;
    }

    void DescriptorWriter::Write()
    {
        vkUpdateDescriptorSets(Context::GetDevice()->GetLogicalDevice(), (std::uint32_t)m_DescriptorWrites.size(), m_DescriptorWrites.data(), 0, nullptr);
    }
}