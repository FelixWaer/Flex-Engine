#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <string>

#include "FXE_Window.h"
#include "FXE_GraphicPipeline.h"
#include "FXE_DebugMessenger.h"
#include "FXE_FrameCreation.h"
#include "FXE_VertexBuffer.h"
#include "FXE_TextureImage.h"

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

    //Methods
    void create_Instance();
    void pick_PhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    static bool check_ValidationLayerSupport();
    void create_LogicalDevice();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    uint32_t find_MemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    //Extension Support Methods
    static std::vector<const char*> get_RequiredExtensions();
    static void check_ForExtensionsSupport();
    static bool check_DeviceExtensionSupport(VkPhysicalDevice device);

    //Variables
    FXEWindow TheWindow;
    FXEGraphicPipeline TheGraphicPipeline;
    FXEDebugMessenger TheDebugMessenger;
    FXEFrameCreation TheFrameCreation;
    FXEVertexBuffer TheVertexBuffer;
    FXETextureImage TheTextureImage;

    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const std::string Model_Path = "Code Files/Models/viking_room.obj";
    const std::string Texture_Path = "Code Files/Textures/viking_room.png";
};
