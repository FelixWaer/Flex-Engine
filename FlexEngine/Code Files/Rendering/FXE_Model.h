#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../FXE_ExtraFunctions.h"
#include "Modules/FXE_Mesh.h"

class Model
{
public:
	Model();
	~Model() = default;

	//std::string Model_Path = "Code Files/Models/pen.obj";
	//std::string Texture_Path = "Code Files/Textures/texture2.jpg";

	//std::vector<FXE::Vertex> Vertices;
	//std::vector<uint32_t> Indices;
	//VkBuffer VertexBuffer;
	//VkBuffer IndexBuffer;
	//VkDeviceMemory VertexBufferMemory;
	//VkDeviceMemory IndexBufferMemory;

	FlexMesh* MeshPtr;

	glm::mat4 get_ModelMatrix();
	glm::vec3 get_Position();
	glm::vec3 get_Rotation();
	void update_Position(const glm::vec3& positionVector);
	void update_Rotation(const glm::vec3& rotationVector);
	int get_TextureID() const;
	void set_TextureID(int textureID);
private:
	glm::vec3 ModelPosition;
	glm::vec3 ModelRotation;
	int TextureID = 1;
};