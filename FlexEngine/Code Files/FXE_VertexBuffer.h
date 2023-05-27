#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <string>

#include "FXE_ExtraFunctions.h"

class FXEFrameCreation;
class FXETextureImage;

class FXEVertexBuffer
{
public:
	void init_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue,
		uint32_t maxFramesInFlight, const std::string& modelPath, FXEFrameCreation* theFrameCreation, FXETextureImage* theTextureImage);
	void cleanup(VkDevice device);

	void create_VertexCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void create_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
	void create_IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
	void create_DescriptorSetLayout(VkDevice device);
	void create_UniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice);
	void create_DescriptorPool(VkDevice device);
	void create_DescriptorSets(VkDevice device);

	static void create_Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static uint32_t find_MemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static VkCommandBuffer begin_SingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	static void end_SingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);

	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;
	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;
	VkCommandPool VertexCommandPool;

	std::vector<VkBuffer> UniformBuffers;
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;
	std::vector<VkDescriptorSet> DescriptorSets;

	std::vector<FXE::Vertex> Vertices;
	std::vector<uint32_t> Indices;

private:
	void copy_Buffer(VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void load_Model(const std::string& modelPath);

	FXEFrameCreation* TheFrameCreationPtr;
	FXETextureImage* TheTextureImagePtr;

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;

	uint32_t MaxFramesInFlight;
};

