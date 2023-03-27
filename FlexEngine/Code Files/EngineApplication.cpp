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

#include <glm/glm.hpp>

#ifdef NDEBUG
const bool enableValidationLayers = false;
const bool debugMode = false;
#else
const bool enableValidationLayers = true;
const bool debugMode = true;
#endif


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
    createInstance();
    TheDebugMessenger.setup_DebugMessenger(Instance);
    TheWindow.createSurface(Instance);
    pickPhysicalDevice();
    createLogicalDevice();
    TheFrameCreation.create_SwapChain(PhysicalDevice, Device, TheWindow.Surface, TheWindow.Window);
    TheFrameCreation.create_ImageViews(Device);
    TheGraphicPipeline.init_GraphicsPipeline(Device, TheFrameCreation.SwapChainImageFormat);
    TheFrameCreation.create_FrameBuffer(Device, TheGraphicPipeline.RenderPass);
    createCommandPool();
    createVertexBuffer();
    createCommandBuffers();
    createSyncObjects();    

}


void FlexEngine::mainLoop()
{

	while (!TheWindow.windowClosing())
	{
		glfwPollEvents();
        drawFrame();
	}
    vkDeviceWaitIdle(Device);
}

void FlexEngine::cleanup()
{
    TheFrameCreation.cleanup(Device);

    vkDestroyBuffer(Device, VertexBuffer, nullptr);
    vkFreeMemory(Device, VertexBufferMemory, nullptr);
    TheGraphicPipeline.cleanup(Device);

    for (size_t i = 0; i < maxFramesInFlight; i++)
    {
        vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(Device, InFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(Device, CommandPool, nullptr);
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

void FlexEngine::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(PhysicalDevice);

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(Device, &commandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pools!");
    }
}

void FlexEngine::createCommandBuffers()
{
    CommandBuffers.resize(maxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) CommandBuffers.size();

    if (vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffer!");
    }
}

void FlexEngine::recordCommandBuffer(VkCommandBuffer theCommandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(theCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = TheGraphicPipeline.RenderPass;
    renderPassBeginInfo.framebuffer = TheFrameCreation.SwapChainFramebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = { TheFrameCreation.SwapChainExtent };

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(theCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(theCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, TheGraphicPipeline.GraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(TheFrameCreation.SwapChainExtent.width);
    viewport.height = static_cast<float>(TheFrameCreation.SwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(theCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = TheFrameCreation.SwapChainExtent;
    vkCmdSetScissor(theCommandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { VertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(theCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(theCommandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(theCommandBuffer);

    if (vkEndCommandBuffer(theCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }


}

void FlexEngine::drawFrame()
{
    vkWaitForFences(Device, 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(Device, TheFrameCreation.SwapChain, UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        TheFrameCreation.recreate_SwapChain(Device, PhysicalDevice, TheGraphicPipeline.RenderPass, &TheWindow);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(Device, 1, &InFlightFences[currentFrame]);

    vkResetCommandBuffer(CommandBuffers[currentFrame], 0);
    recordCommandBuffer(CommandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphore[] = { ImageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(GraphicsQueue, 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { TheFrameCreation.SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(PresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferRezised)
    {
        framebufferRezised = false;
    	TheFrameCreation.recreate_SwapChain(Device, PhysicalDevice, TheGraphicPipeline.RenderPass, &TheWindow);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image");
    }

    currentFrame = (currentFrame + 1) % maxFramesInFlight;
}

void FlexEngine::createSyncObjects()
{
    ImageAvailableSemaphores.resize(maxFramesInFlight);
    RenderFinishedSemaphores.resize(maxFramesInFlight);
    InFlightFences.resize(maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < maxFramesInFlight; i++)
    {
        if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(Device, &fenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

void FlexEngine::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<FlexEngine*>(glfwGetWindowUserPointer(window));
    app->framebufferRezised = true;
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
