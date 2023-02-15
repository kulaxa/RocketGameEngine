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
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return frameBufferResized; }
		void resetWindowResizedFlag(){ frameBufferResized = false; }


		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool frameBufferResized = false;

		std::string windowName;

		GLFWwindow *window;
	};
}