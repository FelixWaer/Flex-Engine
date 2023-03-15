#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class vector;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    /*Main Methods*/
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    /*Methods*/
    void createInstance();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    void setupDebugMessenger();

    /*Variables*/
    GLFWwindow* window;

    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
};
