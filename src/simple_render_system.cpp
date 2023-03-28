#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>

namespace rocket {
	struct SimplePushConstantData {
		glm::mat2 transform{ 0.5f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(RocketDevice& device, VkRenderPass renderPass) : rocketDevice(device)
	{
		createPipelineLayout();
		createPipeline(renderPass);

	}
	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(rocketDevice.device(), pipelineLayout, nullptr);
	}
	
	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(rocketDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Pipeline Layout!");
		}
	}
	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Pipeline layout must be created before pipeline!");
		PipelineConfigInfo pipelineConfig{};
		RocketPipeline::defaultPipelineConfigInfo(pipelineConfig); // Swap chain width and height not always equal to screens
		pipelineConfig.renderPass = renderPass; // Default render pass
		pipelineConfig.pipelineLayout = pipelineLayout;
		rocketPipeline = std::make_unique<RocketPipeline>(rocketDevice, vertShaderPath, fragShaderPath, pipelineConfig);
	}


	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<RocketGameObject>& gameObjects)
	{
		rocketPipeline->bind(commandBuffer);
		for (auto& gameObject : gameObjects) {
			//gameObject.transform2d.rotation = glm::mod(gameObject.transform2d.rotation + 0.01f, glm::two_pi<float>()); // 90 degrees, using radians
			//gameObject.transform2d.translation.y += 0.01f;
			SimplePushConstantData push{};
			push.offset = gameObject.transform2d.translation;
			push.color = gameObject.color;
			push.transform = gameObject.transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			gameObject.model->bind(commandBuffer);
			gameObject.model->draw(commandBuffer);

		}
	}

}
