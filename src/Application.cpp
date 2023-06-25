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

        Renderer2D* renderer = new Renderer2D(m_RenderSystem->GetSwapRenderPass());
        renderer->SetResolution(1280, 720);

        Window::SetResizedCallback([renderer](GLFWwindow* window, int width, int height) 
        {
            renderer->SetResolution(width, height);
        });

        float lastFrameTime = 0;

        while(!Context::GetWindow()->ShouldClose())
        {
            glfwPollEvents();

            float time = (float)glfwGetTime();
            Timestep timestep = time - lastFrameTime;
            lastFrameTime = time;

            if (VkCommandBuffer cmdBuffer = m_RenderSystem->BeginFrame())
            {
                m_RenderSystem->BeginRenderPass(cmdBuffer);
                renderer->Begin();

                renderer->DrawRectangle(Rectangle(0, 0, 50, 50), Vector3(1.0f, 0.4f, 0.1f));
                renderer->DrawRectangle(Rectangle(100, 100, 50, 50), Vector3(0.2f, 0.8f, 0.2f));
                renderer->DrawRectangle(Rectangle(78, 150, 80, 150), Vector3(0.2f, 0.8f, 0.9f));
                renderer->DrawRectangle(Rectangle(900, 450, 200, 200), Vector3(0.3f, 0.1f, 0.6f));

                renderer->End(cmdBuffer, m_RenderSystem->GetCurrentIndex());
                m_RenderSystem->EndRenderPass(cmdBuffer);
                m_RenderSystem->EndFrame();
            }
        }

        // Cleaning Up

        vkDeviceWaitIdle(Context::GetDevice()->GetLogicalDevice());

        delete renderer;
    }
}