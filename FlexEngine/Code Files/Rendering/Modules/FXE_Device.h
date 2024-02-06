#pragma once
#include <vulkan/vulkan_core.h>

class FlexDevice
{
public:
	FlexDevice() = default;

	void init_Device();

	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
private:
	void pick_PhysicalDevice();
};

