#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../FXE_ExtraFunctions.h"


class Model
{
public:
	Model();
	~Model() = default;

	std::string Model_Path = "Code Files/Models/pen.obj";
	std::string Texture_Path = "Code Files/Textures/texture2.jpg";

	std::vector<FXE::Vertex> Vertices;
	std::vector<uint32_t> Indices;
	VkBuffer VertexBuffer;
	VkBuffer IndexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkDeviceMemory IndexBufferMemory;

	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;
	std::vector<VkDescriptorSet> DescriptorSets;


	std::vector<VkBuffer> UniformBuffers;
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;

	VkPipeline GraphicsPipeline;
	VkPipelineLayout PipelineLayout;

	glm::vec3 get_Position();
	glm::vec3 get_Rotation();
	void update_Position(const glm::vec3& positionVector);
	void update_Rotation(const glm::vec3& rotationVector);
	void update_UniformBuffer();
private:
	glm::vec3 ModelPosition;
	glm::vec3 ModelRotation;
};