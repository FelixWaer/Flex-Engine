#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class FXE_Window
{
public:
	FXE_Window(int width, int height, const char* windowName, void* pointer, GLFWframebuffersizefun callback);

	void destroyWindow();
	bool windowClosing();
	void windowMinimized();

	GLFWwindow* window;
private:

};

