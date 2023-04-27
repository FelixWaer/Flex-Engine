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

class FXEFrameCreation
{
public:
	/*Main Methods*/
	//methods for initalizing frame creation, cleaning up frame creation and for drawing the frame
	void init_FrameCreation(FXEWindow* theWindow, FXEGraphicPipeline* theGraphicPipeline, FXEVertexBuffer* theVertexBuffer);
	void cleanup_SwapChain(VkDevice device);
	void cleanup_Semaphores(VkDevice device);
	void draw_Frame(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue presentQueue, VkQueue graphicsQueue);

	/*Public Methods*/
	//Methods for creating different objects needed to draw the frame
	void create_SwapChain(VkPhysicalDevice physicalDevice, VkDevice device);
	void create_ImageViews(VkDevice device);
	void create_FrameBuffer(VkDevice device);
	void create_CommandPool(VkDevice device, VkPhysicalDevice physcialDevice);
	void create_CommandBuffer(VkDevice device);
	void create_SyncObjects(VkDevice device);

	uint32_t get_MaxFramesInFlight() const;

	/*Public Variables*/
	VkFormat SwapChainImageFormat;
	bool FramebufferRezised = false;

private:
	/*Private Methods*/
	void recreate_SwapChain(VkDevice device, VkPhysicalDevice physicalDevice);
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

	//swap chain variables
	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	//command variables
	std::vector<VkCommandBuffer> CommandBuffers;
	VkCommandPool CommandPool;

	//Semaphores Variables
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	std::vector<VkFence> InFlightFences;

	uint32_t CurrentFrame = 0;
	const uint32_t MaxFramesInFlight = 2;
};

