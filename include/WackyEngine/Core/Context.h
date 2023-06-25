#ifndef WACKYENGINE_CORE_CONTEXT_H_
#define WACKYENGINE_CORE_CONTEXT_H_

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Core/Debugger.h"

namespace WackyEngine
{
    struct AppInformation
    {
        const char* AppName;
        const char* EngineName;
        std::uint32_t AppVersion;
        std::uint32_t EngineVersion;
    };

    struct WindowInformation
    {
        std::size_t Width;
        std::size_t Height;
        const char* Title;
    };

    class Context
    {
    private:
        static void InitialiseVulkan(const AppInformation& info);

    public:
        static void Initialise(const AppInformation& appInfo, const WindowInformation& windowInfo);

        static void PrintExtensionInformation();
        static std::vector<const char*> GetRequiredExtensions();

        static VkInstance GetInstance();
        static Device* GetDevice();
        static Window* GetWindow();
        static Debugger* GetDebugger();
    };
}

#endif