#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>


namespace rocket {
	class RocketWindow {
	public:
		RocketWindow(int w, int h, std::string name);
		~RocketWindow();

		RocketWindow(const RocketWindow&) = delete;
		RocketWindow &operator=(const RocketWindow &) = delete; // Disable copying RocketWindow

		bool shouldClose();
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;

		GLFWwindow *window;
	};
}