#pragma once

#include "rocket_window.hpp"
#include "rocket_pipeline.hpp"
#include "rocket_device.hpp"

namespace rocket {

	class TutorialApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		std::string fragShaderPath = "shaders/simple_shader.frag.spv";
		std::string vertShaderPath = "shaders/simple_shader.vert.spv";

		void run();
	private:
		RocketWindow rocketWindow{ WIDTH, HEIGHT, "Rocket" };
		RocketDevice rocketDevice{ rocketWindow };
		RocketPipeline rocketPipeline{ rocketDevice, fragShaderPath, vertShaderPath , RocketPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

	};
}