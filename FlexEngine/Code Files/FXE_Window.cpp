#include "FXE_Window.h"

FXE_Window::FXE_Window(int width, int height, const char* windowName, void* pointer, GLFWframebuffersizefun callback)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
	glfwSetWindowUserPointer(window, pointer);
	glfwSetFramebufferSizeCallback(window, callback);
}

void FXE_Window::destroyWindow()
{
	glfwDestroyWindow(window);
}

bool FXE_Window::windowClosing()
{
	if (glfwWindowShouldClose(window))
	{
		return true;
	}
	return false;
}
