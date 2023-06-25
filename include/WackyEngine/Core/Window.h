#ifndef WACKYENGINE_CORE_WINDOW_H_
#define WACKYENGINE_CORE_WINDOW_H_

#include <string>
#include <functional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace WackyEngine
{
    class Window
    {
    private:
        GLFWwindow* m_GLFWWindow;
        VkSurfaceKHR m_Surface;

        int m_Width;
        int m_Height;
        std::string m_Title;

        bool m_ResizedFlag = false;
        static std::function<void(GLFWwindow*, int, int)> m_ResizeCallbackExtension;
        static void ResizedCallback(GLFWwindow* window, int width, int height);

    public:
        Window(const int width, const int height, const std::string& title);
        ~Window();

        void InitialiseSurface();

        inline bool ShouldClose() { return glfwWindowShouldClose(m_GLFWWindow); }
        inline bool WasResized() { return m_ResizedFlag; }
        inline void ResetResizedFlag() { m_ResizedFlag = false; }
        inline static void SetResizedCallback(std::function<void(GLFWwindow*, int, int)> callback) { m_ResizeCallbackExtension = callback; }

        inline int GetWidth() const noexcept { return m_Width; }
        inline int GetHeight() const noexcept{ return m_Width; }
        inline std::string GetTitle() const noexcept{ return m_Title; }
        inline VkSurfaceKHR GetSurface() const noexcept{ return m_Surface; }
        inline GLFWwindow* GetGLFWWindow() const noexcept { return m_GLFWWindow; }
    };
}

#endif