#ifndef WACKYENGINE_CORE_DEBUGGER_H_
#define WACKYENGINE_CORE_DEBUGGER_H_

#include <vector>

#include <vulkan/vulkan.h>

namespace WackyEngine
{
    class Debugger
    {
    private:
        VkDebugUtilsMessengerEXT m_DebugMessenger; 

        bool CheckValidationLayerSupport();
        
    public:
        const static std::vector<const char*> ValidationLayers;

        Debugger();
        ~Debugger();

        static VkDebugUtilsMessengerCreateInfoEXT GetDefaultInfo();
    };
}

#endif