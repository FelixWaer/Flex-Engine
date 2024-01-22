#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <string>

#include "../FlexLibrary/Flextimer.h"

#include "../FXE_Window.h"
#include "../FXE_DebugMessenger.h"
#include "../FXE_ExtraFunctions.h"
#include "FXE_Model.h"
#include "FXE_Camera.h"

struct MeshPushConstants
{
    glm::vec4 Data;
    glm::mat4 RenderMatrix;
};

class FlexEngine {
public:
    void run() {
        FlexTimer::clear_Timer();
        TheWindow.initWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), "FlexEngine", this, frameBufferResizeCallback);
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
    bool is_DeviceSuitable(VkPhysicalDevice physicalDevice);
    static bool check_ValidationLayerSupport();
    void create_LogicalDevice();
    static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

    uint32_t find_MemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkImageView create_ImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void create_Image(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSample, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    //Extension Support Methods
    static std::vector<const char*> get_RequiredExtensions();
    static void check_ForExtensionsSupport();
    static bool check_DeviceExtensionSupport(VkPhysicalDevice device);

    //Graphic Pipeline Methods
    void init_GraphicsPipeline();
    void cleanup_GraphicsPipeline();
    void create_GraphicsPipeline(Model* fxeModel);
    void create_RenderPass(VkFormat swapChainImageFormat);
    VkShaderModule create_ShaderModule(const std::vector<char>& data);

    static std::vector<char> readFile(const char* filename);

    //Frame Creation Methods
    void cleanup_SwapChain();
    void cleanup_Semaphores();
    void draw_Frame();

    void create_SwapChain();
    void create_ImageViews();
    void create_FrameBuffer();
    void create_CommandPool();
    void create_DepthResources();
    void create_CommandBuffer();
    void create_SyncObjects();

    void recreate_SwapChain();
    void record_CommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    static VkSurfaceFormatKHR choose_SwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    static VkPresentModeKHR choose_SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    static VkExtent2D choose_SwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

    //Texture Image Methods
    void cleanup_TextureImage();

    void create_TextureImage(const std::string& texturePath);
    void create_TextureImageView();
    void create_TextureSampler();

    void transition_ImageLayout(VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, uint32_t mipLevels);
    void copy_BufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void generate_Mipmaps(VkImage texImage, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    //Vertex Buffer Methods
    void init_VertexBuffer();
    void cleanup_VertexBuffer();

    void create_VertexCommandPool();
    void load_Model(Model* modelPtr);
    void create_VertexBuffer(Model* modelPtr);
    void create_IndexBuffer(Model* modelPtr);
    void create_DescriptorSetLayout(Model* fxeModel);
    void create_UniformBuffers(Model* fxeModel);
    void create_DescriptorPool(Model* fxeModel);
    void create_DescriptorSets(Model* fxeModel);

    void copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void create_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void end_SingleTimeCommands(VkCommandBuffer commandBuffer);
    VkCommandBuffer begin_SingleTimeCommands();

    //Multisampling Methods
    VkSampleCountFlagBits get_MaxUsableSampleCount();
    void create_ColorResources();

    //Variables
    FXEWindow TheWindow;
    FXEDebugMessenger TheDebugMessenger;

    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;

    //Graphics Pipeline Variables
    //VkPipeline GraphicsPipeline;
    VkRenderPass RenderPass;
    VkPipelineLayout MeshPipelineLayout;

    //Frame Creation Variables
    VkFormat SwapChainImageFormat;
    VkCommandPool CommandPool;

    VkSwapchainKHR SwapChain;
    std::vector<VkImage> SwapChainImages;
    VkExtent2D SwapChainExtent;
    std::vector<VkImageView> SwapChainImageViews;
    std::vector<VkFramebuffer> SwapChainFrameBuffers;

    std::vector<VkCommandBuffer> CommandBuffers;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;

    VkImage DepthImage;
    VkDeviceMemory DepthImageMemory;
    VkImageView DepthImageView;

    uint32_t CurrentFrame = 0;
    const uint32_t MaxFramesInFlight = 2;
    bool FramebufferRezised = false;

    //Texture Image Variables
    VkImage TextureImage;
    VkDeviceMemory TextureImageMemory;
    VkImageView TextureImageView;
    VkSampler TextureSampler;

    uint32_t MipLevels;

    //VkDescriptorSetLayout DescriptorSetLayout;
    //VkDescriptorPool DescriptorPool;
    VkCommandPool VertexCommandPool;

    //std::vector<VkBuffer> UniformBuffers;
    //std::vector<VkDeviceMemory> UniformBuffersMemory;
    //std::vector<void*> UniformBuffersMapped;
    //std::vector<VkDescriptorSet> DescriptorSets;

    //Multisampling
    VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    VkImage ColorImage;
    VkDeviceMemory ColorImageMemory;
    VkImageView ColorImageView;

    //Other Variables
    static const uint32_t WIDTH = 800;
    static const uint32_t HEIGHT = 600;

    const std::string Model_Path = "Code Files/Models/pen.obj";
    const std::string Texture_Path = "Code Files/Textures/texture2.jpg";
    const std::string Model_Path_2 = "Code Files/Models/viking_room.obj";
    const std::string Texture_Path_2 = "Code Files/Textures/texture2.jpg";

    Model Model_1;
    Model Model_2;
    Model Model_3;
};
