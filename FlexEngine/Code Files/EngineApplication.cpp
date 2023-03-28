#include "EngineApplication.h"

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
/*-------------Structs-------------*/
/*---------------------------------*/
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{

    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

/*---------------------------------*/
/*----------Const Vector-----------*/
/*---------------------------------*/

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

/*---------------------------------*/
/*----------Main Methods-----------*/
/*---------------------------------*/

void FlexEngine::initVulkan()
{
    TIME(auto start)
	TheFrameCreation.init_FrameCreation(&TheWindow, &TheGraphicPipeline);
    createInstance();
    TheDebugMessenger.setup_DebugMessenger(Instance);
    TheWindow.createSurface(Instance);
    pickPhysicalDevice();
    createLogicalDevice();
    TheFrameCreation.create_SwapChain(PhysicalDevice, Device);
    TheFrameCreation.create_ImageViews(Device);
    TheGraphicPipeline.init_GraphicsPipeline(Device, TheFrameCreation.SwapChainImageFormat);
    TheFrameCreation.create_FrameBuffer(Device);
    TheFrameCreation.create_CommandPool(Device, findQueueFamilies(PhysicalDevice));
    createVertexBuffer();
    TheFrameCreation.create_CommandBuffer(Device);
    TheFrameCreation.create_SyncObjects(Device);
    TIME(auto end)
	PRINT_TIME_MS("time to initialize Vulkan: ", start, end)
}


void FlexEngine::mainLoop()
{

	while (!TheWindow.windowClosing())
	{
        TIME(auto start)
		glfwPollEvents();
        TheFrameCreation.draw_Frame(Device, PhysicalDevice, PresentQueue, GraphicsQueue, VertexBuffer, static_cast<uint32_t>(vertices.size()));
        TIME(auto end)
		PRINT_TIME_NS("ms per frame ", start, end)
	}
    vkDeviceWaitIdle(Device);
}

void FlexEngine::cleanup()
{
    TheFrameCreation.cleanup_SwapChain(Device);

    vkDestroyBuffer(Device, VertexBuffer, nullptr);
    vkFreeMemory(Device, VertexBufferMemory, nullptr);
    TheGraphicPipeline.cleanup(Device);
    TheFrameCreation.cleanup_Semaphores(Device);
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
void FlexEngine::createInstance()
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
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
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
void FlexEngine::pickPhysicalDevice()
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
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    //checks if GPU supports swap chain and swap chain support drawing on our surface
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    bool swapChainAdequate = false;
	if (extensionsSupported)
	{
        SwapChainSupportDetails swapChainSupport = FXEFrameCreation::query_SwapChainSupport(physicalDevice, TheWindow.Surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

//Finds the queue families in the GPU that is needed
QueueFamilyIndices FlexEngine::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
	    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	    {
            indices.graphicsFamily = i;
	    }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, TheWindow.Surface, &presentSupport);

	    if (presentSupport)
	    {
            indices.presentFamily = i;
	    }

	    if (indices.isComplete())
	    {
            break;
	    }
        i++;
    }

    return indices;
}

void FlexEngine::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(PhysicalDevice);

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
        checkForExtensionsSupport();
    }
}

bool FlexEngine::checkValidationLayerSupport()
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
    auto app = reinterpret_cast<FlexEngine*>(glfwGetWindowUserPointer(window));
    app->TheFrameCreation.FramebufferRezised = true;
}

void FlexEngine::createVertexBuffer()
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(Device, &bufferInfo, nullptr, &VertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(Device, VertexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(Device, &memoryAllocInfo, nullptr, &VertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(Device, VertexBuffer, VertexBufferMemory, 0);

    void* data;
    vkMapMemory(Device, VertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferInfo.size);
    vkUnmapMemory(Device, VertexBufferMemory);
}

uint32_t FlexEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
	    if ((typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
	    {
            return i;
	    }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

/*---------------------------------*/
/*-------Extensions Methods--------*/
/*---------------------------------*/

//gets the required extensions needed
std::vector<const char*> FlexEngine::getRequiredExtensions()
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

void FlexEngine::checkForExtensionsSupport()
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
bool FlexEngine::checkDeviceExtensionSupport(VkPhysicalDevice device)
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
