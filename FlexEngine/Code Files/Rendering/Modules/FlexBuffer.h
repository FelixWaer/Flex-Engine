#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct BufferCreateInfo
{
	VkDeviceSize BufferSize;
	VkBufferUsageFlags usage;
	VkBufferUsageFlags usage_2;
	VkMemoryPropertyFlags properties;
	VkMemoryPropertyFlags properties_2;
	void* Data;
};

class FlexBuffer
{
public:
	FlexBuffer() = default;

	VkBuffer Buffer;
	VkDeviceMemory BufferMemory;
	VkDeviceSize BufferSize;

	void init_Buffer(BufferCreateInfo& bufferInfo);
	void cleanup_Buffer();
private:

	void create_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t find_MemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkCommandBuffer begin_SingleTimeCommands();
	void end_SingleTimeCommands(VkCommandBuffer commandBuffer);

};

