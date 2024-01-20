#pragma once
#include <string>
#include <vector>

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
private:
};