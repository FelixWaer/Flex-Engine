#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class FXEVertexBuffer
{
public:
	void init_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue);
	void cleanup(VkDevice device);

	void create_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
	void create_VertexCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	uint32_t VertexCount;
private:
	static void create_Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copy_Buffer(VkDevice device, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static uint32_t find_MemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandPool VertexCommandPool;
};

