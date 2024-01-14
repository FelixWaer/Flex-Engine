#include "../FXE_Window.h"

#include <iostream>

#include "../FlexLibrary/Flextimer.h"

void FXEWindow::initWindow(int width, int height, const char* windowName, void* pointer, GLFWframebuffersizefun callback)
{
	FlexTimer timer("window initializing");
	Width = width;
	Height = height;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	Window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
	glfwSetWindowUserPointer(Window, pointer);
	glfwSetFramebufferSizeCallback(Window, callback);
}

void FXEWindow::cleanup()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

bool FXEWindow::windowClosing()
{
	if (glfwWindowShouldClose(Window))
	{
		return true;
	}
	return false;
}

void FXEWindow::windowMinimized()
{
	glfwGetFramebufferSize(Window, &Width, &Height);
	while (Width == 0 || Height == 0)
	{
		glfwGetFramebufferSize(Window, &Width, &Height);
		glfwWaitEvents();
	}
}

void FXEWindow::createSurface(VkInstance theInstance)
{
	if (glfwCreateWindowSurface(theInstance, Window, nullptr, &Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

