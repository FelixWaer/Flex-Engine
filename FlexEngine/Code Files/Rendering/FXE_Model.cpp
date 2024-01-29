#include "FXE_Model.h"

#include <chrono>

#include "FXE_RendererManager.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);

    ModelPosition = glm::vec3(0.f);
    ModelRotation = glm::vec3(0.f);
    ModelScale = glm::vec3(1.0f);
}

glm::mat4 Model::get_ModelMatrix()
{
    glm::mat4 modelMatrix = glm::rotate(glm::translate(glm::mat4(1.0f), ModelPosition), glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, ModelScale);
    return modelMatrix;
}

glm::mat4 Model::get_ModelMatrixInstance(glm::vec3 positionVector)
{
    glm::mat4 modelMatrix = glm::rotate(glm::translate(glm::mat4(1.0f), positionVector), glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, ModelScale);
    return modelMatrix;
}

void Model::cleanup_Model()
{
}

glm::vec3 Model::get_Position()
{
	return ModelPosition;
}

glm::vec3 Model::get_Rotation()
{
    return ModelRotation;
}

glm::vec3 Model::get_Scale()
{
    return ModelScale;
}

void Model::update_Position(const glm::vec3& positionVector)
{
    ModelPosition = positionVector;
}

void Model::update_Rotation(const glm::vec3& rotationVector)
{
    ModelRotation = rotationVector;
}

void Model::update_Scale(const glm::vec3& scaleVector)
{
    ModelScale = scaleVector;
}

int Model::get_TextureID() const
{
    return TextureID;
}

void Model::set_TextureID(int textureID)
{
    TextureID = textureID;
}