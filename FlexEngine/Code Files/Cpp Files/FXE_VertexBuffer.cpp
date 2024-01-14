#include "../FXE_VertexBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <unordered_map>

#include "../FXE_ExtraFunctions.h"
#include "../FXE_TextureImage.h"
#include "../FXE_FrameCreation.h"
#include "../FlexLibrary/Flextimer.h"

/*---------------------------------*/
/*---------Public Methods----------*/
/*---------------------------------*/

void FXEVertexBuffer::init_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue, 
    uint32_t maxFramesInFlight, const std::string& modelPath, FXEFrameCreation* theFrameCreation , FXETextureImage* theTextureImage)
{
    FlexTimer timer("Vertex Buffer Initializing");

    Device = device;
    PhysicalDevice = physicalDevice;
    TheFrameCreationPtr = theFrameCreation;
    TheTextureImagePtr = theTextureImage;
    MaxFramesInFlight = maxFramesInFlight;

    load_Model(modelPath);
    create_VertexCommandPool(device, physicalDevice, surface);
    create_VertexBuffer(device, physicalDevice, graphicsQueue);
    create_IndexBuffer(device, physicalDevice, graphicsQueue);
    create_UniformBuffers(device, physicalDevice);
    create_DescriptorPool(device);
    create_DescriptorSets(device);
}

void FXEVertexBuffer::cleanup(VkDevice device)
{
    vkDestroyBuffer(device, IndexBuffer, nullptr);
    vkFreeMemory(device, IndexBufferMemory, nullptr);
    vkDestroyBuffer(device, VertexBuffer, nullptr);
    vkFreeMemory(device, VertexBufferMemory, nullptr);
    vkDestroyCommandPool(device, VertexCommandPool, nullptr);
    vkDestroyDescriptorPool(device, DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, DescriptorSetLayout, nullptr);
    for (uint32_t i = 0; i < MaxFramesInFlight; i++)
    {
        vkDestroyBuffer(device, UniformBuffers[i], nullptr);
        vkFreeMemory(device, UniformBuffersMemory[i], nullptr);
    }
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

void FXEVertexBuffer::create_VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue)
{
    VkDeviceSize bufferSize = sizeof(Vertices[0]) * Vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, Vertices.data(),bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    
    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

    copy_Buffer(graphicsQueue, stagingBuffer, VertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void FXEVertexBuffer::create_IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue)
{
    VkDeviceSize bufferSize = sizeof(Indices[0]) * Indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, Indices.data(), bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);

    copy_Buffer(graphicsQueue, stagingBuffer, IndexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void FXEVertexBuffer::create_DescriptorSetLayout(VkDevice device)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void FXEVertexBuffer::create_UniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice)
{
    VkDeviceSize bufferSize = sizeof(FXE::UniformBufferObject);

    UniformBuffers.resize(MaxFramesInFlight);
    UniformBuffersMemory.resize(MaxFramesInFlight);
    UniformBuffersMapped.resize(MaxFramesInFlight);

    for (uint32_t i = 0; i < MaxFramesInFlight; i++)
    {
        create_Buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffers[i], UniformBuffersMemory[i]);

        vkMapMemory(device, UniformBuffersMemory[i], 0, bufferSize, 0, &UniformBuffersMapped[i]);
    }
}

void FXEVertexBuffer::create_DescriptorPool(VkDevice device)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = MaxFramesInFlight;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = MaxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = MaxFramesInFlight;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void FXEVertexBuffer::create_DescriptorSets(VkDevice device)
{
    std::vector<VkDescriptorSetLayout> layouts(MaxFramesInFlight, DescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocDescriptorInfo{};
    allocDescriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocDescriptorInfo.descriptorPool = DescriptorPool;
    allocDescriptorInfo.descriptorSetCount = MaxFramesInFlight;
    allocDescriptorInfo.pSetLayouts = layouts.data();

    DescriptorSets.resize(MaxFramesInFlight);
    if (vkAllocateDescriptorSets(device, &allocDescriptorInfo, DescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (uint32_t i = 0; i < MaxFramesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = UniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FXE::UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = TheTextureImagePtr->TextureImageView;
        imageInfo.sampler = TheTextureImagePtr->TextureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = DescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = DescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

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

/*---------------------------------*/
/*---------Private Methods---------*/
/*---------------------------------*/

void FXEVertexBuffer::copy_Buffer(VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = FXE::begin_SingleTimeCommands(Device, TheFrameCreationPtr->CommandPool);
    
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    FXE::end_SingleTimeCommands(Device, TheFrameCreationPtr->CommandPool, commandBuffer, graphicsQueue);
}

void FXEVertexBuffer::load_Model(const std::string& modelPath)
{
    FlexTimer timer("Model loading");

    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, modelPath.c_str()))
    {
        throw std::runtime_error(warning + error);
    }

    std::unordered_map<FXE::Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
	    for (const auto& index : shape.mesh.indices)
	    {
            FXE::Vertex vertex{};

            vertex.pos = {
                attribute.vertices[3 * index.vertex_index + 0],
                attribute.vertices[3 * index.vertex_index + 1],
                attribute.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoordinates = {
                attribute.texcoords[2 * index.texcoord_index + 0],
                1.0f - attribute.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.contains(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(Vertices.size());
                Vertices.push_back(vertex);
            }

            Indices.push_back(uniqueVertices[vertex]);
	    }
    }
}
