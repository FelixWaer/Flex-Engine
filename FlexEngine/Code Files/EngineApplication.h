#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>

#include "FXE_Window.h"
#include "FXE_GraphicPipeline.h"
#include "FXE_DebugMessenger.h"
#include "FXE_FrameCreation.h"
#include "FXE_VertexBuffer.h"

class FlexEngine {
public:
    void run() {
        TheWindow.initWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), "FlexEngine", this, framebufferResizeCallback);
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    //Main Methods
	void initVulkan();

    void mainLoop();

    void cleanup();

    //Method
    void createInstance();
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    static bool checkValidationLayerSupport();
    void createLogicalDevice();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    //Extension Support Methods
    static std::vector<const char*> getRequiredExtensions();
    static void checkForExtensionsSupport();
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);


    //Variables
    FXEWindow TheWindow;
    FXEGraphicPipeline TheGraphicPipeline;
    FXEDebugMessenger TheDebugMessenger;
    FXEFrameCreation TheFrameCreation;
    FXEVertexBuffer TheVertexBuffer;

    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
};
