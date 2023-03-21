#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <string>

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

class FlexEngine {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

    //Main Methods
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    //Method
    void createInstance();
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    static bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void createLogicalDevice();
    void createSurface();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer theCommandBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObjects();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    //Swap Chain Methods
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapChain();
    void recreateSwapChain();
    void cleanupSwapChain();

    //Graphic Pipeline Methods
    void createGraphicsPipeline();
    static std::vector<char> readFile(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    //Extension Support Methods
    static std::vector<const char*> getRequiredExtensions();
    static void checkForExtensionsSupport();
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    //Debug Methods
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    void setupDebugMessenger();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance theInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT
    (VkInstance theInstance, VkDebugUtilsMessengerEXT theDebugMessenger,
        const VkAllocationCallbacks* pAllocator);


    //Variables
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    bool framebufferRezised = false;

    //Swap Chain Variables
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;


    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const int maxFramesInFlight = 2;
    uint32_t currentFrame = 0;

};
