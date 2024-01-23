#include "FlexBuffer.h"

#include <iostream>

#include "../FXE_RendererManager.h"

void FlexBuffer::init_Buffer(BufferCreateInfo& bufferInfo)
{
	BufferSize = bufferInfo.BufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_Buffer(BufferSize, bufferInfo.usage, bufferInfo.properties, stagingBuffer, stagingBufferMemory);


    void* data;
    vkMapMemory(FXE::EngineInformation.Device, stagingBufferMemory, 0, BufferSize, 0, &data);
    memcpy(data, bufferInfo.Data, BufferSize);
    vkUnmapMemory(FXE::EngineInformation.Device, stagingBufferMemory);

    create_Buffer(BufferSize, bufferInfo.usage_2, bufferInfo.properties_2, Buffer, BufferMemory);

    copy_Buffer(stagingBuffer, Buffer, BufferSize);

    vkDestroyBuffer(FXE::EngineInformation.Device, stagingBuffer, nullptr);
    vkFreeMemory(FXE::EngineInformation.Device, stagingBufferMemory, nullptr);
}

void FlexBuffer::cleanup_Buffer()
{
    vkDestroyBuffer(FXE::EngineInformation.Device, Buffer, nullptr);
    vkFreeMemory(FXE::EngineInformation.Device, BufferMemory, nullptr);
}

void FlexBuffer::create_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(FXE::EngineInformation.Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(FXE::EngineInformation.Device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = find_MemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(FXE::EngineInformation.Device, &memoryAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(FXE::EngineInformation.Device, buffer, bufferMemory, 0);
}

void FlexBuffer::copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = begin_SingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    end_SingleTimeCommands(commandBuffer);
}

uint32_t FlexBuffer::find_MemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(FXE::EngineInformation.PhysicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

VkCommandBuffer FlexBuffer::begin_SingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = FXE::EngineInformation.CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(FXE::EngineInformation.Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void FlexBuffer::end_SingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(FXE::EngineInformation.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(FXE::EngineInformation.GraphicsQueue);

    vkFreeCommandBuffers(FXE::EngineInformation.Device, FXE::EngineInformation.CommandPool, 1, &commandBuffer);
}
