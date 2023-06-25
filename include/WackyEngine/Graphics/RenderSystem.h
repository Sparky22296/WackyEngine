#ifndef WACKYENGINE_GRAPHICS_RENDERSYSTEM_H_
#define WACKYENGINE_GRAPHICS_RENDERSYSTEM_H_

#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Graphics/SwapChain.h"
#include "WackyEngine/Math/Vector3.h"

namespace WackyEngine
{
    class RenderSystem
    {
    private:
        bool m_FrameStarted;
        std::uint32_t m_CurrentIndex;
        Vector3 m_ClearColour;

        SwapChain* m_SwapChain;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        void InitialiseCommandBuffers();

    public:
        RenderSystem();
        ~RenderSystem();
        
        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginRenderPass(VkCommandBuffer buffer);
        void EndRenderPass(VkCommandBuffer buffer);

        inline void SetClearColour(const Vector3& colour) { m_ClearColour = colour; }

        inline bool IsFrameStarted() const noexcept { return m_FrameStarted; }
        inline std::uint32_t GetCurrentIndex() const noexcept { return m_CurrentIndex; }
        inline RenderPass* GetSwapRenderPass() const noexcept { return m_SwapChain->GetRenderPass(); }
        inline SwapChain* GetSwapChain() const noexcept { return m_SwapChain; }

    };
}

#endif