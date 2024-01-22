#pragma once
#include "../../FXE_ExtraFunctions.h"

class FlexMesh
{
public:
	FlexMesh() = default;
	FlexMesh(std::string modelPath);

	std::string ModelPath;

	std::vector<FXE::Vertex> Vertices;
	std::vector<uint32_t> Indices;
	VkBuffer VertexBuffer;
	VkBuffer IndexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkDeviceMemory IndexBufferMemory;

	void init_Mesh(std::string modelPath);
private:
	void load_Mesh();
};

