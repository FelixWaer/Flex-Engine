#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>

struct SwapChainSupportDetails;
struct QueueFamilyIndices;

class FXEWindow;
class FXEGraphicPipeline;
class FXEVertexBuffer;
class FXETextureImage;

class FXEFrameCreation
{
public:
	/*Main Methods*/
	//methods for initalizing frame creation, cleaning up frame creation and for drawing the frame
	void init_FrameCreation(VkDevice device, VkPhysicalDevice physicalDevice, FXEWindow* theWindow, FXEGraphicPipeline* theGraphicPipeline, FXEVertexBuffer* theVertexBuffer, FXETextureImage* theTextureImage);
	void cleanup_SwapChain();
	void cleanup_Semaphores();
	void draw_Frame(VkQueue presentQueue, VkQueue graphicsQueue);

	/*Public Methods*/
	//Methods for creating different objects needed to draw the frame
	void create_SwapChain();
	void create_ImageViews();
	void create_FrameBuffer();
	void create_CommandPool();
	void create_DepthResources(VkQueue graphicsQueue);
	void create_CommandBuffer();
	void create_SyncObjects();

	uint32_t get_MaxFramesInFlight() const;

	/*Public Variables*/
	VkFormat SwapChainImageFormat;
	VkCommandPool CommandPool;

	bool FramebufferRezised = false;

private:
	/*Private Methods*/
	void recreate_SwapChain(VkQueue graphicsQueue);
	void record_CommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void update_UniformBuffer(uint32_t currentImage);

	static VkSurfaceFormatKHR choose_SwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
	static VkPresentModeKHR choose_SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D choose_SwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	/*Private Variables*/
	//Pointers to the the window and graphic pipeline class to get easier access to its variables
	FXEWindow* TheWindowPtr;
	FXEGraphicPipeline* TheGraphicPipelinePtr;
	FXEVertexBuffer* TheVertexBufferPtr;
	FXETextureImage* TheTextureImagePtr;

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;

	//swap chain variables
	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	//command variables
	std::vector<VkCommandBuffer> CommandBuffers;

	//Semaphores Variables
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	std::vector<VkFence> InFlightFences;

	//Depth image variables
	VkImage DepthImage;
	VkDeviceMemory DepthImageMemory;
	VkImageView DepthImageView;

	uint32_t CurrentFrame = 0;
	const uint32_t MaxFramesInFlight = 2;
};

