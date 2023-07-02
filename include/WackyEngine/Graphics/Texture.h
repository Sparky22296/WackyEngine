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

    public:
        Texture(const std::string& fileName);
        ~Texture();

        inline VkImageView GetImageView() const noexcept { return m_TextureImageView; }
    };
}

#endif