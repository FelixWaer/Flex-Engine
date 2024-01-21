#include "FXE_Camera.h"

#include <glm/ext/matrix_transform.hpp>

void FlexCamera::rotate_Camera()
{
	
}

glm::mat4 FlexCamera::get_CameraView()
{
	return glm::lookAt(CameraEye, CameraCenter, CameraUp);
}
