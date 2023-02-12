#include "rocket_window.hpp"
#include <stdexcept>
namespace rocket {

	RocketWindow::RocketWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
}
	RocketWindow::~RocketWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	bool RocketWindow::shouldClose()
	{
		return glfwWindowShouldClose(window);
	}
	void RocketWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}
	void RocketWindow::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable resizing

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

}
