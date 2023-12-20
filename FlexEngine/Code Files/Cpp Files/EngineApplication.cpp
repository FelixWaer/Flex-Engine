#include "../EngineApplication.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>

#include <glm/glm.hpp>

#include "../FXE_ExtraFunctions.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
const bool debugMode = false;
#else
const bool enableValidationLayers = true;
const bool debugMode = true;
#endif

#define TIME(x) x = std::chrono::high_resolution_clock::now();
#define PRINT_TIME_NS(text, start, end) std::cout << text << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;
#define PRINT_TIME_MS(text, start, end) std::cout << text << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

/*---------------------------------*/
/*----------Const Vector-----------*/
/*---------------------------------*/

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

/*---------------------------------*/
/*----------Main Methods-----------*/
/*---------------------------------*/

void FlexEngine::initVulkan()
{
    TIME(auto start)

    create_Instance();
    TheDebugMessenger.setup_DebugMessenger(Instance);
    TheWindow.createSurface(Instance);
    pick_PhysicalDevice();
    create_LogicalDevice();
    TheFrameCreation.init_FrameCreation(Device, PhysicalDevice, &TheWindow, &TheGraphicPipeline, &TheVertexBuffer, &TheTextureImage);
    TheFrameCreation.create_SwapChain();
    TheFrameCreation.create_ImageViews();
    TheVertexBuffer.create_DescriptorSetLayout(Device);
    TheGraphicPipeline.init_GraphicsPipeline(Device, PhysicalDevice, TheFrameCreation.SwapChainImageFormat, TheVertexBuffer.DescriptorSetLayout);
    TheFrameCreation.create_CommandPool();
    TheTextureImage.init_TextureImage(Device, PhysicalDevice, GraphicsQueue, &TheFrameCreation, &TheVertexBuffer);
    TheFrameCreation.create_DepthResources(GraphicsQueue);
    TheFrameCreation.create_FrameBuffer();
    TheTextureImage.create_TextureImage(Texture_Path);
    TheTextureImage.create_TextureImageView();
    TheTextureImage.create_TextureSampler();
    TheVertexBuffer.init_VertexBuffer(Device, PhysicalDevice, TheWindow.Surface, GraphicsQueue, TheFrameCreation.get_MaxFramesInFlight(), Model_Path,
        &TheFrameCreation, &TheTextureImage);
    TheFrameCreation.create_CommandBuffer();
    TheFrameCreation.create_SyncObjects();

    TIME(auto end)
	PRINT_TIME_MS("time to initialize Vulkan: ", start, end)
}

void FlexEngine::mainLoop()
{

	while (!TheWindow.windowClosing())
	{
		glfwPollEvents();
        TheFrameCreation.draw_Frame(PresentQueue, GraphicsQueue);
	}
    vkDeviceWaitIdle(Device);
}

void FlexEngine::cleanup()
{
    TheFrameCreation.cleanup_SwapChain();

    TheTextureImage.cleanup();
    TheVertexBuffer.cleanup(Device);
    TheGraphicPipeline.cleanup(Device);
    TheFrameCreation.cleanup_Semaphores();

    vkDestroyDevice(Device, nullptr);

	if (enableValidationLayers)
	{
        TheDebugMessenger.cleanup(Instance);
	}

    vkDestroySurfaceKHR(Instance, TheWindow.Surface, nullptr);
	vkDestroyInstance(Instance, nullptr);

    TheWindow.cleanup();
}

/*---------------------------------*/
/*-------------Methods-------------*/
/*---------------------------------*/
void FlexEngine::create_Instance()
{
	if (enableValidationLayers && !check_ValidationLayerSupport())
	{
        throw std::runtime_error("validation layers requested, but not available!");
	}

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "FLex Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = get_RequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        TheDebugMessenger.populate_DebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
	}

    VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);

    if (vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

//Picks what GPU to use
void FlexEngine::pick_PhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to load GPU with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
	    if (isDeviceSuitable(device))
	    {
            PhysicalDevice = device;
            break;
	    }
    }

    if (PhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

//Checks if the GPU's queue families are suitable
bool FlexEngine::isDeviceSuitable(VkPhysicalDevice physicalDevice)
{
	//find queue families and checks if its suitable
    FXE::QueueFamilyIndices indices = FXE::find_QueueFamilies(physicalDevice, TheWindow.Surface);

    //checks if GPU supports swap chain and swap chain support drawing on our surface
    bool extensionsSupported = check_DeviceExtensionSupport(physicalDevice);

    bool swapChainAdequate = false;
	if (extensionsSupported)
	{
        FXE::SwapChainSupportDetails swapChainSupport = FXE::query_SwapChainSupport(physicalDevice, TheWindow.Surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);


    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void FlexEngine::create_LogicalDevice()
{
    FXE::QueueFamilyIndices indices = FXE::find_QueueFamilies(PhysicalDevice, TheWindow.Surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(Device, indices.graphicsFamily.value(), 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, indices.presentFamily.value(), 0, &PresentQueue);

    if (debugMode)
    {
        check_ForExtensionsSupport();
    }
}

bool FlexEngine::check_ValidationLayerSupport()
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

void FlexEngine::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = static_cast<FlexEngine*>(glfwGetWindowUserPointer(window));
    app->TheFrameCreation.FramebufferRezised = true;
}

/*---------------------------------*/
/*-------Extensions Methods--------*/
/*---------------------------------*/

//gets the required extensions needed
std::vector<const char*> FlexEngine::get_RequiredExtensions()
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

void FlexEngine::check_ForExtensionsSupport()
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


//Checks if the GPU supports Swap Chain to draw on the screen
bool FlexEngine::check_DeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
