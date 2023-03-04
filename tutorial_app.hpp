#pragma once

#include "rocket_window.hpp"
#include "rocket_device.hpp"
#include "rocket_renderer.hpp"
#include <memory>
#include <vector>
#include "rocket_model.hpp"
#include "rocket_game_object.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


namespace rocket {

	class TutorialApp {
	public:
		static constexpr int WIDTH = 1500;
		static constexpr int HEIGHT = 1000;
		std::string fragShaderPath = "shaders/simple_shader.frag.spv";
		std::string vertShaderPath = "shaders/simple_shader.vert.spv";

		TutorialApp();
		~TutorialApp();

		TutorialApp(const TutorialApp&) = delete;
		TutorialApp &operator=(const TutorialApp &) = delete; // Disable copying TutorialApp

		void run();
	private:
		void loadGameObjects();

		RocketWindow rocketWindow{ WIDTH, HEIGHT, "Rocket" };
		RocketDevice rocketDevice{ rocketWindow };
		RocketRenderer rocketRenderer{ rocketWindow, rocketDevice };
		std::vector<RocketGameObject> gameObjects;
	};
}