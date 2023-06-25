#ifndef WACKYENGINE_GRAPHICS_TEXTURE_H_
#define WACKYENGINE_GRAPHICS_TEXTURE_H_

#include <string>

#include "WackyEngine/Core/Device.h"

namespace WackyEngine
{
    class Texture
    {
    private:
        VkImageView m_TextureImageView;
        VkImage m_TextureImage;
        VkDeviceMemory m_TextureImageMemory;
        VkSampler m_TextureSampler;

        void InitialiseSampler();

    public:
        Texture() { }
        ~Texture() { }

        void Initialise(const std::string& fileName);
        void Destroy();

        inline VkImageView GetImageView() const noexcept { return m_TextureImageView; }
        inline VkSampler GetSampler() const noexcept { return m_TextureSampler; }
    };
}

#endif