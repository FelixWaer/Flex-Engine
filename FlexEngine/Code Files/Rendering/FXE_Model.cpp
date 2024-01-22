#include "FXE_Model.h"

#include <chrono>

#include "FXE_RendererManager.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);

    ModelPosition = glm::vec3(0.f);
    ModelRotation = glm::vec3(0.f);
}

glm::mat4 Model::get_ModelMatrix()
{
    glm::mat4 modelMatrix = glm::rotate(glm::translate(glm::mat4(1.0f), ModelPosition), glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    return modelMatrix;
}

glm::vec3 Model::get_Position()
{
	return ModelPosition;
}

glm::vec3 Model::get_Rotation()
{
    return ModelRotation;
}

void Model::update_Position(const glm::vec3& positionVector)
{
    ModelPosition = positionVector;
}

void Model::update_Rotation(const glm::vec3& rotationVector)
{
    ModelRotation = rotationVector;
}
