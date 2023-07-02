#ifndef WACKYENGINE_APPLICATION_H_
#define WACKYENGINE_APPLICATION_H_

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Window.h"
#include "WackyEngine/Core/Debugger.h"
#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Graphics/RenderSystem.h"
#include "WackyEngine/Graphics/Model.h"

namespace WackyEngine
{
    struct FrameData
    {
        VkCommandBuffer CmdBuffer;
        std::uint32_t FrameIndex;
    };

    class Application
    {
    private:
        RenderSystem* m_RenderSystem;

    protected:
        inline RenderSystem* GetRenderSystem() { return m_RenderSystem; }

    public:
        Application(const int width, const int height, const std::string& windowTitle);
        ~Application();

        void Run();

        virtual void Initialise() = 0;
        virtual void Update() = 0;
        virtual void Draw(const FrameData& frameData) = 0;
        virtual void OnWindowResize(int newWidth, int newHeight) { }
    };
}

#endif