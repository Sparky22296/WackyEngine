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
    class Application
    {
    private:
        RenderSystem* m_RenderSystem;

    public:
        Application(const int width, const int height, const std::string& windowTitle);
        ~Application();

        void Run();

        void OnWindowResize(GLFWwindow* window, int width, int height);

        // virtual void Initialise() = 0;
        // virtual void Update() = 0;
        // virtual void Draw() = 0;
    };
}

#endif