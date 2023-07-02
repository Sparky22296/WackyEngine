#include "WackyEngine/Application.h"

#include <stdexcept>
#include <iostream>

#include <GLFW/glfw3.h>

#include "WackyEngine/Core/Debugger.h"
#include "WackyEngine/Core/Timestep.h"
#include "WackyEngine/Core/Context.h"
#include "WackyEngine/Math/Matrix4.h"
#include "WackyEngine/Graphics/Renderers/Renderer2D.h"
#include "WackyEngine/Graphics/UniformBufferObject.h"
#include "WackyEngine/Graphics/SwapChain.h"
#include "WackyEngine/Graphics/DescriptorUtil.h"

#define print(x) std::cout << x << std::endl

namespace WackyEngine
{
    Application::Application(const int width, const int height, const std::string& windowTitle)
    {   
        AppInformation appInfo { };
        appInfo.AppName = "WackyEngine App";
        appInfo.AppVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.EngineName = "WackyEngine";
        appInfo.EngineVersion = VK_MAKE_VERSION(1, 0, 0);

        WindowInformation windowInfo { };
        windowInfo.Width = width;
        windowInfo.Height = height;
        windowInfo.Title = windowTitle.c_str();

        Context::Initialise(appInfo, windowInfo);

        m_RenderSystem = new RenderSystem();
    }

    Application::~Application()
    {
        delete m_RenderSystem;
    }

    void Application::Run()
    {
        m_RenderSystem->SetClearColour({0, 0, 0});

        Window::SetResizedCallback([this](GLFWwindow* window, int width, int height) 
        {
            OnWindowResize(width, height);
        });

        float lastFrameTime = 0;

        Initialise();

        while(!Context::GetWindow()->ShouldClose())
        {
            glfwPollEvents();

            float time = (float)glfwGetTime();
            Timestep timestep = time - lastFrameTime;
            lastFrameTime = time;

            Update();

            if (VkCommandBuffer cmdBuffer = m_RenderSystem->BeginFrame())
            {
                m_RenderSystem->BeginRenderPass(cmdBuffer);

                FrameData data;
                data.CmdBuffer = cmdBuffer;
                data.FrameIndex = m_RenderSystem->GetCurrentIndex();

                Draw(data);

                m_RenderSystem->EndRenderPass(cmdBuffer);
                m_RenderSystem->EndFrame();
            }
        }

        // Cleaning Up

        vkDeviceWaitIdle(Context::GetDevice()->GetLogicalDevice());
    }
}