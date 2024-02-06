#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>

#include <vector>

class FlexPipeline
{
public:
	FlexPipeline() = default;

	void init_Pipeline(VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkSampleCountFlagBits msaaSamples);
	void cleanup_Pipeline();

	VkPipeline GraphicsPipeline;
	VkPipelineLayout PipelineLayout;

private:
	void create_DefaultPipelineSettings(VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkSampleCountFlagBits msaaSamples);
	VkShaderModule create_ShaderModule(const std::vector<char>& data);
	std::vector<char> read_File(const char* filename);
};

