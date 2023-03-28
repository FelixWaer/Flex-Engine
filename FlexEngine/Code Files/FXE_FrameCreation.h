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

class FXEFrameCreation
{
public:
	void init_FrameCreation(FXEWindow* theWindow, FXEGraphicPipeline* theGraphicPipeline);
	void cleanup_SwapChain(VkDevice device);
	void cleanup_Semaphores(VkDevice device);
	void draw_Frame(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue presentQueue, VkQueue graphicsQueue, VkBuffer vertexBuffer, uint32_t vertexCount);

	void create_SwapChain(VkPhysicalDevice physicalDevice, VkDevice device);
	void create_ImageViews(VkDevice device);
	void create_FrameBuffer(VkDevice device);
	void create_CommandPool(VkDevice device, QueueFamilyIndices indices);
	void create_CommandBuffer(VkDevice device);
	void create_SyncObjects(VkDevice device);

	static SwapChainSupportDetails query_SwapChainSupport(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);

	VkFormat SwapChainImageFormat;
	bool FramebufferRezised = false;

private:
	void recreate_SwapChain(VkDevice device, VkPhysicalDevice physicalDevice);
	void record_CommandBuffer(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t imageIndex, VkBuffer vertexBuffer);

	static VkSurfaceFormatKHR choose_SwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
	static VkPresentModeKHR choose_SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D choose_SwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	static QueueFamilyIndices find_QueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	FXEWindow* TheWindowPtr;
	FXEGraphicPipeline* TheGraphicPipelinePtr;

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

	int CurrentFrame = 0;
	const int MaxFramesInFlight = 2;
};

