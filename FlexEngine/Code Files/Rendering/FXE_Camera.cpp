#include "FXE_Camera.h"

#include <glm/ext/matrix_transform.hpp>
#include "FXE_RendererManager.h"

void FlexCamera::update_Camera(glm::vec3 positionVector)
{
	CameraCenter += positionVector;
	CameraEye += positionVector;
	CameraEye.y = CameraCenter.y + CameraDistance;
}

void FlexCamera::update_CameraDistance(float distance)
{
	CameraDistance += distance;
	if (CameraDistance < 0.1f)
	{
		CameraDistance = 0.1f;
	}
	CameraEye.y = CameraCenter.y + CameraDistance;
	
}

void FlexCamera::rotate_Camera(glm::vec3 rotationVector)
{

}

glm::mat4 FlexCamera::get_CameraView()
{
	return glm::lookAt(CameraEye, CameraCenter, CameraUp);
}

glm::mat4 FlexCamera::get_CameraProjection()
{
	return glm::perspective(glm::radians(90.0f), static_cast<float>(FXE::Width) / static_cast<float>(FXE::Height), 0.1f, 200.0f);
}
