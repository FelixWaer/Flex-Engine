#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class FXEWindow
{
public:
	FXEWindow(){}

	void initWindow(int width, int height, const char* windowName, void* pointer, GLFWframebuffersizefun callback);
	void cleanup();
	bool windowClosing();
	void windowMinimized();

	void createSurface(VkInstance theInstance);

	GLFWwindow* Window;
	VkSurfaceKHR Surface;
private:
	int Width;
	int Height;
};

