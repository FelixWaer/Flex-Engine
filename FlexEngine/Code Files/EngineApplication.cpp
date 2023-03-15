#include "EngineApplication.h"

#include <iostream>
#include <cstring>
#include <vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void checkForExtensionsSupport()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionsSupport(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsSupport.data());

    std::cout << "available extension:\n";
    for (const auto& extensions : extensionsSupport)
    {
        std::cout << '\t' << extensions.extensionName << '\n';
    }
}

/*Validation checking*/
const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}


/*Methods Definition*/
void HelloTriangleApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    
}

void HelloTriangleApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
}


void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void HelloTriangleApplication::cleanup()
{
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

void HelloTriangleApplication::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
        throw std::runtime_error("validation layers requested, but not available!");
	}

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "FLex Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers)
	{
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
        createInfo.enabledLayerCount = 0;
	}

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    checkForExtensionsSupport();
}

VkBool32 HelloTriangleApplication::debugCallback
   (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void HelloTriangleApplication::setupDebugMessenger()
{
    if (!enableValidationLayers) return;
}
