#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>

struct Vertex;

class FXEGraphicPipeline
{
public:
	void init_GraphicsPipeline(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat swapChainImageFormat, VkDescriptorSetLayout& descriptorSetLayout);
	void cleanup(VkDevice device);

	VkPipeline GraphicsPipeline;
	VkRenderPass RenderPass;
	VkPipelineLayout PipelineLayout;
private:
	void create_GraphicsPipeline(VkDevice device, VkDescriptorSetLayout& descriptorSetLayout);
	void create_RenderPass(VkDevice device, VkFormat swapChainImageFormat);

	static VkShaderModule create_ShaderModule(VkDevice device, const std::vector<char>& data);
	static std::vector<char> readFile(const char* filename);

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
};
