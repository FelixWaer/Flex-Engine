#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class FXEVertexBuffer;
class FXEFrameCreation;

class FXETextureImage
{
public:
	void init_TextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, FXEFrameCreation* theFrameCreation, FXEVertexBuffer* theVertexBuffer);
	void cleanup();

	void create_TextureImage(VkQueue graphicsQueue, const std::string& texturePath);
	void create_TextureImageView();
	void create_TextureSampler();

	void transition_ImageLayout(VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);

	VkImage TextureImage;
	VkDeviceMemory TextureImageMemory;
	VkImageView TextureImageView;
	VkSampler TextureSampler;

private:

	void copy_BufferToImage(VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;

	FXEFrameCreation* TheFrameCreationPtr;
	FXEVertexBuffer* TheVertexBufferPtr;
};

