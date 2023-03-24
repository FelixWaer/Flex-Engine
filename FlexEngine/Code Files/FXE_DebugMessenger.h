#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



class FXEDebugMessenger
{
public:
	void setup_DebugMessenger(VkInstance instance);
	static void populate_DebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& utilsMessengerCreateInfo);
	void cleanup(VkInstance instance);

private:
	static VkBool32 debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance theInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void destroyDebugUtilsMessengerEXT(VkInstance theInstance, VkDebugUtilsMessengerEXT theDebugMessenger,
		const VkAllocationCallbacks* pAllocator);


	VkDebugUtilsMessengerEXT DebugMessenger;

};