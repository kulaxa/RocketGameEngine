#pragma once

#include "rocket_pipeline.hpp"
#include "rocket_device.hpp"

#include <memory>
#include <vector>
#include "rocket_model.hpp"
#include "rocket_game_object.hpp"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


namespace rocket {

	class SimpleRenderSystem {
	public:
		std::string fragShaderPath = "shaders/simple_shader.frag.spv";
		std::string vertShaderPath = "shaders/simple_shader.vert.spv";

		SimpleRenderSystem(RocketDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete; // Disable copying TutorialApp

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<RocketGameObject> &gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		RocketDevice &rocketDevice;
		std::unique_ptr<RocketPipeline> rocketPipeline;
		VkPipelineLayout pipelineLayout;
	};
}