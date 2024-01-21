#pragma once
#include <vector>
#include "FXE_Model.h"
#include "FXE_Camera.h"

namespace FXE
{
	inline std::vector<Model*> ModelManager;
	inline FlexCamera Camera;
	inline uint32_t CurrentFrame = 0;
	inline uint32_t Width;
	inline uint32_t Height;
}
