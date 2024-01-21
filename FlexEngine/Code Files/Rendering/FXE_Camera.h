#pragma once
#include <glm/glm.hpp>

class FlexCamera
{
public:
	FlexCamera() = default;

	void update_Camera(float right, float forward, float up)
	{
		CameraEye.x += right;
		CameraEye.y += forward;
		CameraEye.z += up;

		CameraCenter.x += right;
		CameraCenter.y += forward;
		CameraCenter.z += up;
	}
	void turn_Around()
	{
		CameraUp.z *= -1;
	}

	void rotate_Camera();

	glm::mat4 get_CameraView();

private:
	glm::vec3 CameraEye = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 CameraCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
};