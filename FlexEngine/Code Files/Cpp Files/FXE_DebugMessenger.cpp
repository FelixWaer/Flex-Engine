#include "../FXE_DebugMessenger.h"

#include <iostream>

#ifdef NDEBUG
const bool debugMode = false;
#else
const bool debugMode = true;
#endif

void FXEDebugMessenger::setup_DebugMessenger(VkInstance instance)
{
	if (!debugMode) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfoMessenger;
	populate_DebugMessengerCreateInfo(createInfoMessenger);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfoMessenger, nullptr, &DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void FXEDebugMessenger::populate_DebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& utilsMessengerCreateInfo)
{
	utilsMessengerCreateInfo = {};

	utilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	utilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	utilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	utilsMessengerCreateInfo.pfnUserCallback = debugCallback;
}

void FXEDebugMessenger::cleanup(VkInstance instance)
{
	if (!debugMode) return;

	destroyDebugUtilsMessengerEXT(instance, DebugMessenger, nullptr);
}

VkBool32 FXEDebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VkResult FXEDebugMessenger::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void FXEDebugMessenger::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

