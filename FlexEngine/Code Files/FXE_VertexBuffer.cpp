#include "FXE_VertexBuffer.h"

#include <iostream>
#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "ExtraFunctions.h"

const std::vector<FXE::Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

/*---------------------------------*/
/*---------Public Methods----------*/
/*---------------------------------*/

void FXEVertexBuffer::init_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue)
{
    create_VertexCommandPool(device, physicalDevice, surface);
    create_VertexBuffer(device, physicalDevice, graphicsQueue);
}

void FXEVertexBuffer::cleanup(VkDevice device)
{
    vkDestroyBuffer(device, VertexBuffer, nullptr);
    vkFreeMemory(device, VertexBufferMemory, nullptr);
    vkDestroyCommandPool(device, VertexCommandPool, nullptr);
}

void FXEVertexBuffer::create_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue)
{
    VertexCount = static_cast<uint32_t>(vertices.size());

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(),bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

    copy_Buffer(device, graphicsQueue, stagingBuffer, VertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void FXEVertexBuffer::create_VertexCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    FXE::QueueFamilyIndices indices = FXE::find_QueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    commandPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &VertexCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

/*---------------------------------*/
/*---------Private Methods---------*/
/*---------------------------------*/

void FXEVertexBuffer::create_Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& vertexBuffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = find_MemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &memoryAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device, vertexBuffer, bufferMemory, 0);
}

void FXEVertexBuffer::copy_Buffer(VkDevice device, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = VertexCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, VertexCommandPool, 1, &commandBuffer);
}

uint32_t FXEVertexBuffer::find_MemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}