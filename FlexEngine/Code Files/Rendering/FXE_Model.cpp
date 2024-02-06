#include "FXE_Model.h"

#include <chrono>

#include "FXE_RendererManager.h"
#include "EngineApplication.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);

    ModelPosition = glm::vec3(0.f);
    ModelRotation = glm::vec3(0.f);
    ModelScale = glm::vec3(1.0f);
}

void Model::draw_Model(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    pushData testcolor2;
    testcolor2.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    testcolor2.model = get_ModelMatrix();
    testcolor2.textureIndex = get_TextureID();

    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushData), &testcolor2);

    VkBuffer vertexBuffers[] = { MeshPtr->VertexBuffer.Buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, MeshPtr->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MeshPtr->Indices.size()), 1, 0, 0, 0);
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

FlexMesh* Model::get_Mesh()
{
    return MeshPtr;    
}

void Model::set_Mesh(FlexMesh* meshPtr)
{
    MeshPtr = meshPtr;
}

int Model::get_TextureID() const
{
    return TextureID;
}

void Model::set_TextureID(int textureID)
{
    TextureID = textureID;
}
