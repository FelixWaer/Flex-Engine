#include "FXE_Camera.h"

#include <glm/ext/matrix_transform.hpp>

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

glm::mat4 FlexCamera::get_CameraView()
{
	return glm::lookAt(CameraEye, CameraCenter, CameraUp);
}
