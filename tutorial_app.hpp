#pragma once

#include "rocket_window.hpp"
#include "rocket_pipeline.hpp"
#include "rocket_device.hpp"
#include "rocket_swap_chain.hpp"
#include <memory>
#include <vector>
#include "rocket_model.hpp"
#include "rocket_game_object.hpp"

namespace rocket {

	class TutorialApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		std::string fragShaderPath = "shaders/simple_shader.frag.spv";
		std::string vertShaderPath = "shaders/simple_shader.vert.spv";

		TutorialApp();
		~TutorialApp();

		TutorialApp(const TutorialApp&) = delete;
		TutorialApp &operator=(const TutorialApp &) = delete; // Disable copying TutorialApp

		void run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void loadGameObjects();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		RocketWindow rocketWindow{ WIDTH, HEIGHT, "Rocket" };
		RocketDevice rocketDevice{ rocketWindow };
		std::unique_ptr<RocketSwapChain> rocketSwapChain;
		std::unique_ptr<RocketPipeline> rocketPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<RocketGameObject> gameObjects;
	};
}