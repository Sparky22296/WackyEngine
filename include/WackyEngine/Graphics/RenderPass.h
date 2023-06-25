#ifndef WACKYENGINE_GRAPHICS_RENDERPASS_H_
#define WACKYENGINE_GRAPHICS_RENDERPASS_H_

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Device.h"

namespace WackyEngine
{
    class RenderPass
    {
    private:
        VkRenderPass m_RenderPass;

    public:
        static RenderPass* CreateSimplePass(VkFormat format);

        RenderPass() { }
        ~RenderPass();

        inline VkRenderPass GetRenderPass() const noexcept { return m_RenderPass; }
    };
}

#endif