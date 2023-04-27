#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>

class FXEVertexBuffer
{
public:
	void init_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue, uint32_t maxFramesInFlight);
	void cleanup(VkDevice device);

	void create_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
	void create_IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
	void create_VertexCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void create_DescriptorSetLayout(VkDevice device);
	void create_UniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice);
	void create_DescriptorPool(VkDevice device);
	void create_DescriptorSets(VkDevice device);

	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;
	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;

	std::vector<VkBuffer> UniformBuffers;
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;
	std::vector<VkDescriptorSet> DescriptorSets;

private:
	static void create_Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copy_Buffer(VkDevice device, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static uint32_t find_MemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandPool VertexCommandPool;
	uint32_t MaxFramesInFlight;
};

