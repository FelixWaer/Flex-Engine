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

	void draw_Model(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

	glm::mat4 get_ModelMatrix();
	glm::mat4 get_ModelMatrixInstance(glm::vec3 positionVector);
	glm::vec3 get_Position();
	glm::vec3 get_Rotation();
	glm::vec3 get_Scale();
	void update_Position(const glm::vec3& positionVector);
	void update_Rotation(const glm::vec3& rotationVector);
	void update_Scale(const glm::vec3& scaleVector);
	FlexMesh* get_Mesh();
	void set_Mesh(FlexMesh* meshPtr);
	int get_TextureID() const;
	void set_TextureID(int textureID);

	FlexMesh* MeshPtr;
private:
	glm::vec3 ModelPosition;
	glm::vec3 ModelRotation;
	glm::vec3 ModelScale;
	int TextureID = 1;
	int ID = 0;
};