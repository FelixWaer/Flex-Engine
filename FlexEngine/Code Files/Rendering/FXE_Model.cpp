#include "FXE_Model.h"

#include <chrono>

#include "FXE_RendererManager.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);
}

//void Model::update_Uniformbuffer()
//{
//    static auto startTime = std::chrono::high_resolution_clock::now();
//
//    auto currentTime = std::chrono::high_resolution_clock::now();
//    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//
//    FXE::UniformBufferObject ubo{};
//    if (forward <= 1.0f)
//    {
//        forward = 1.0f;
//    }
//    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(rotateZAxes)), glm::vec3(0.0f, 0.0f, 1.0f)) *
//        glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(rotateYAxes)), glm::vec3(1.0f, 0.0f, 0.0f));
//    ubo.view = glm::lookAt(glm::vec3(right, forward, up), glm::vec3(right, 1.0f, up), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.proj = glm::perspective(glm::radians(90.0f), static_cast<float>(SwapChainExtent.width) / static_cast<float>(SwapChainExtent.height), 0.1f, 100.0f);
//    ubo.proj[1][1] *= -1;
//
//    memcpy(UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
//}
