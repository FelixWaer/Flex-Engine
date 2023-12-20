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

	void create_TextureImage(const std::string& texturePath);
	void create_TextureImageView();
	void create_TextureSampler();

	void transition_ImageLayout(VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, uint32_t mipLevels);

	VkImage TextureImage;
	VkDeviceMemory TextureImageMemory;
	VkImageView TextureImageView;
	VkSampler TextureSampler;

	uint32_t MipLevels;

private:

	void copy_BufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generate_Mipmaps(VkImage texImage, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
	VkQueue GraphicsQueue;

	FXEFrameCreation* TheFrameCreationPtr;
	FXEVertexBuffer* TheVertexBufferPtr;
};

