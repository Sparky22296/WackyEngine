#include "WackyEngine/Graphics/Texture.h"

#include <stdexcept>
#include <iostream>

#include <vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include "WackyEngine/Vendor/stb_image.h"

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    Texture::Texture(const std::string &fileName)
    {
        int width, height, channels;
        stbi_uc* pixels = stbi_load(fileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        VkDeviceSize imageSize = width * height * 4;

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image.");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        Context::GetDevice()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(Context::GetDevice()->GetLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, (std::size_t)imageSize);
        vkUnmapMemory(Context::GetDevice()->GetLogicalDevice(), stagingBufferMemory);

        stbi_image_free(pixels);

        Context::GetDevice()->CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);
    
        Context::GetDevice()->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        Context::GetDevice()->CopyBufferToImage(stagingBuffer, m_TextureImage, (std::uint32_t)width, (std::uint32_t)height);
        Context::GetDevice()->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(Context::GetDevice()->GetLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(Context::GetDevice()->GetLogicalDevice(), stagingBufferMemory, nullptr);

        // Image View

        VkImageViewCreateInfo viewInfo { };
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_TextureImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(Context::GetDevice()->GetLogicalDevice(), &viewInfo, nullptr, &m_TextureImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view.");
        }

        // InitialiseSampler();
    }

    Texture::~Texture()
    {
        // vkDestroySampler(Context::GetDevice()->GetLogicalDevice(), m_TextureSampler, nullptr);
        vkDestroyImageView(Context::GetDevice()->GetLogicalDevice(), m_TextureImageView, nullptr);
        vkDestroyImage(Context::GetDevice()->GetLogicalDevice(), m_TextureImage, nullptr);
        vkFreeMemory(Context::GetDevice()->GetLogicalDevice(), m_TextureImageMemory, nullptr);
    }
    
    // void Texture::InitialiseSampler()
    // {
    //     VkSamplerCreateInfo info { };
    //     info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //     info.magFilter = VK_FILTER_LINEAR;
    //     info.minFilter = VK_FILTER_LINEAR;
    //     info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //     info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //     info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //     info.anisotropyEnable = VK_TRUE;

    //     VkPhysicalDeviceProperties properties { };
    //     vkGetPhysicalDeviceProperties(Context::GetDevice()->GetPhysicalDevice(), &properties);

    //     info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    //     info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    //     info.unnormalizedCoordinates = VK_FALSE;
    //     info.compareEnable = VK_FALSE;
    //     info.compareOp = VK_COMPARE_OP_ALWAYS;
    //     info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //     info.mipLodBias = 0.0f;
    //     info.minLod = 0.0f;
    //     info.maxLod = 0.0f;

    //     if (vkCreateSampler(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &m_TextureSampler) != VK_SUCCESS)
    //     {
    //         throw std::runtime_error("Failed to create texture sampler.");
    //     }
    // }
}