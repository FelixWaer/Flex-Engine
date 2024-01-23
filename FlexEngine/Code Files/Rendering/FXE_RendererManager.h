#pragma once
#include <vector>
#include "FXE_Model.h"
#include "Modules/FXE_Mesh.h"
#include "FXE_Camera.h"


namespace FXE
{
	struct EngineInfo
	{
		VkDevice Device;
		VkPhysicalDevice PhysicalDevice;
		VkQueue GraphicsQueue;
		VkCommandPool CommandPool;
	};

	inline EngineInfo EngineInformation ;
	inline std::vector<Model*> ModelManager;
	inline std::vector<FlexMesh*> MeshManager;
	inline FlexCamera Camera;
	inline uint32_t CurrentFrame = 0;
	inline uint32_t Width;
	inline uint32_t Height;
}
