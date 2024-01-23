#pragma once
#include "../../FXE_ExtraFunctions.h"
#include "FlexBuffer.h"

class FlexMesh
{
public:
	FlexMesh() = default;
	FlexMesh(std::string modelPath);

	std::string ModelPath;

	std::vector<FXE::Vertex> Vertices;
	std::vector<uint32_t> Indices;

	FlexBuffer VertexBuffer;
	FlexBuffer IndexBuffer;

	void init_Mesh(std::string modelPath);
	void cleanup_Mesh();
private:
	void load_Mesh();
};

