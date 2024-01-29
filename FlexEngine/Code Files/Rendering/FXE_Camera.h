#pragma once
#include <glm/glm.hpp>

class FlexCamera
{
public:
	FlexCamera() = default;

	void update_Camera(glm::vec3 positionVector);
	void update_CameraDistance(float distance);
	void rotate_Camera(glm::vec3 rotationVector);

	glm::mat4 get_CameraView();
	glm::mat4 get_CameraProjection();
	

private:
	glm::vec3 CameraEye = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 CameraCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

	float CameraDistance = 10.f;
};