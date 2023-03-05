#pragma once

#include "rocket_window.hpp"
#include "rocket_device.hpp"
#include "rocket_renderer.hpp"
#include <memory>
#include <vector>
#include "rocket_model.hpp"
#include "rocket_game_object.hpp"

#include "physics_system.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


namespace rocket {

	class TutorialApp {
	public:
		static constexpr int WIDTH = 720;
		static constexpr int HEIGHT = 720;
		std::string fragShaderPath = "shaders/simple_shader.frag.spv";
		std::string vertShaderPath = "shaders/simple_shader.vert.spv";

		TutorialApp();
		~TutorialApp();

		TutorialApp(const TutorialApp&) = delete;
		TutorialApp &operator=(const TutorialApp &) = delete; // Disable copying TutorialApp

		void run();
	private:
		void loadGameObjects();
		uint32_t createParticle(glm::vec2 position);
		uint32_t getSelectedParticle(float xMouse, float yMouse);
		uint32_t getParticleIndex(uint32_t particleId);
		void clearSimulation();
		RocketWindow rocketWindow{ WIDTH, HEIGHT, "Rocket" };
		RocketDevice rocketDevice{ rocketWindow };
		RocketRenderer rocketRenderer{ rocketWindow, rocketDevice };
		std::vector<RocketGameObject> gameObjects;
		PhysicsSystem physicsSystem{ glm::vec2(0.0f, 3.0f) };
		std::shared_ptr<RocketModel> circleModel = nullptr;
	};
}