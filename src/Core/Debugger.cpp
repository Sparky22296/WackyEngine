#include "WackyEngine/Core/Debugger.h"

#include <stdexcept>
#include <iostream>

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    const std::vector<const char*> Debugger::ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    {
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            std::cerr << "Validation Layer: " << callbackData->pMessage << '\n' << std::endl;
        }

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        
        if (func != nullptr) 
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } 
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) 
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        
        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    Debugger::Debugger()
    {
        if (!CheckValidationLayerSupport())
        {
            throw std::runtime_error("Failed to locate validation layers.");
        }

        VkDebugUtilsMessengerCreateInfoEXT info = GetDefaultInfo();

        if (CreateDebugUtilsMessengerEXT(Context::GetInstance(), &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create debug messenger");
        }
    }

    Debugger::~Debugger()
    {
        DestroyDebugUtilsMessengerEXT(Context::GetInstance(), m_DebugMessenger, nullptr);
    }

    bool Debugger::CheckValidationLayerSupport()
    {
        std::uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (std::size_t i = 0; i < ValidationLayers.size(); ++i)
        {
            for (std::size_t j = 0; j < layerCount; ++j)
            {
                if (strcmp(ValidationLayers[i], availableLayers[j].layerName) == 0)
                {
                    goto next_iteration;
                }
            }
            
            return false;

            next_iteration:;
        }

        return true;
    }

    VkDebugUtilsMessengerCreateInfoEXT Debugger::GetDefaultInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT info { };

        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        info.pfnUserCallback = DebugCallback;
        info.pUserData = nullptr;

        return info;
    }
}