#include "FXE_Model.h"

#include <chrono>

#include "FXE_RendererManager.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);

    ModelPosition = glm::vec3(0.f);
    ModelRotation = glm::vec3(0.f);
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

void Model::update_UniformBuffer()
{
    //static auto startTime = std::chrono::high_resolution_clock::now();

    //auto currentTime = std::chrono::high_resolution_clock::now();
    //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    FXE::UniformBufferObject ubo{};

    ubo.model = glm::rotate(glm::translate(glm::mat4(1.0f), ModelPosition), glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.view = FXE::Camera.get_CameraView();
    ubo.proj = glm::perspective(glm::radians(90.0f), static_cast<float>(FXE::Width) / static_cast<float>(FXE::Height), 0.1f, 100.0f);
    ubo.proj[1][1] *= -1;

    memcpy(UniformBuffersMapped[FXE::CurrentFrame], &ubo, sizeof(ubo));
}
