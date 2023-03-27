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

class FXEFrameCreation
{
public:
	void cleanup(VkDevice device);
	void create_SwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow* window);
	void create_ImageViews(VkDevice device);
	void create_FrameBuffer(VkDevice device, VkRenderPass renderPass);
	void recreate_SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderPass, FXEWindow* window);

	static SwapChainSupportDetails query_SwapChainSupport(const VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);

	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
private:
	static VkSurfaceFormatKHR choose_SwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
	static VkPresentModeKHR choose_SwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D choose_SwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	static QueueFamilyIndices find_QueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};

