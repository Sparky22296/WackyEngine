#include "WackyEngine/Core/Window.h"

#include <stdexcept>
#include <iostream>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    std::function<void(GLFWwindow*, int, int)> Window::m_ResizeCallbackExtension;

    void Window::ResizedCallback(GLFWwindow* window, int width, int height)
    {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        win->m_ResizedFlag = true;
        win->m_Width = width;
        win->m_Height = height;

        m_ResizeCallbackExtension(window, width, height);
    }

    Window::Window(const int width, const int height, const std::string& title)
    {
        m_Width = width;
        m_Height = height;
        m_Title = title;

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_GLFWWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_GLFWWindow, this);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, ResizedCallback);
    }

    Window::~Window()
    {
        vkDestroySurfaceKHR(Context::GetInstance(), m_Surface, nullptr);
        glfwDestroyWindow(m_GLFWWindow);
        glfwTerminate();
    }

    void Window::InitialiseSurface()
    {
        if (glfwCreateWindowSurface(Context::GetInstance(), m_GLFWWindow, nullptr, &m_Surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface.");
        }
    }
}