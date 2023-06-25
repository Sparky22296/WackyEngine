#include "WackyEngine/Core/Context.h"

#include <iostream>

namespace WackyEngine
{
    struct ContextData
    {
        VkInstance Instance;

        Device* Device;
        Window* Window;
        Debugger* Debugger;

        ~ContextData()
        {
            delete Debugger;
            delete Window;
            delete Device;

            vkDestroyInstance(Instance, nullptr);
        }
    };

    static ContextData s_Data;

    void Context::Initialise(const AppInformation& appInfo, const WindowInformation& windowInfo)
    {
        s_Data.Window = new Window(windowInfo.Width, windowInfo.Height, windowInfo.Title);
        InitialiseVulkan(appInfo);
        s_Data.Window->InitialiseSurface();
        s_Data.Debugger = new Debugger();
        s_Data.Device = new Device();
    }

    void Context::InitialiseVulkan(const AppInformation& appInfo)
    {
        // Application Info
        VkApplicationInfo applicationInfo { };
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = appInfo.AppName;
        applicationInfo.applicationVersion = appInfo.AppVersion;
        applicationInfo.pEngineName = appInfo.EngineName;
        applicationInfo.engineVersion = appInfo.EngineVersion;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        // Instance Info
        VkInstanceCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &applicationInfo;

        std::vector<const char*> extensions = GetRequiredExtensions();
        info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
        info.ppEnabledExtensionNames = extensions.data();

        // Creation/Destruction Debugger
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = Debugger::GetDefaultInfo();
        info.enabledLayerCount = static_cast<std::uint32_t>(Debugger::ValidationLayers.size());
        info.ppEnabledLayerNames = Debugger::ValidationLayers.data();
        info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;

        if (vkCreateInstance(&info, nullptr, &s_Data.Instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    void Context::PrintExtensionInformation()
    {
        // Available Extensions
        std::uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "Available Extensions:" << std::endl;

        for (std::size_t i = 0; i < extensionCount; ++i)
        {
            std::cout << "\t - " << extensions[i].extensionName << " VERSION " + std::to_string(extensions[i].specVersion) << std::endl;
        }

        // GLFW Extensions
        std::cout << "Used GLFW Extensions:" << std::endl;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
        std::cout << *glfwExtensions << std::endl;
    }

    std::vector<const char*> Context::GetRequiredExtensions()
    {
        std::uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    VkInstance Context::GetInstance()
    {
        return s_Data.Instance;
    }

    Device* Context::GetDevice()
    {
        return s_Data.Device;
    }

    Window* Context::GetWindow()
    {
        return s_Data.Window;
    }

    Debugger* Context::GetDebugger()
    {
        return s_Data.Debugger;
    }
}