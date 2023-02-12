#include "tutorial_app.hpp"
#include "rocket_pipeline.hpp"
#include <iostream>
#include <stdexcept>
#include <array>
namespace rocket {
	TutorialApp::TutorialApp()
	{
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();


	}
	TutorialApp::~TutorialApp()
	{
		vkDestroyPipelineLayout(rocketDevice.device(), pipelineLayout, nullptr);
	}
	void TutorialApp::run()
	{
		std::cout << "Starting Tutorial App." << std::endl;
		while (!rocketWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(rocketDevice.device());
	}
	void TutorialApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(rocketDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Pipeline Layout!");
		}
	}
	void TutorialApp::createPipeline()
	{
		auto pipelineConfig = RocketPipeline::defaultPipelineConfigInfo(rocketSwapChain.width(), rocketSwapChain.height()); // Swap chain width and height not always equal to screens
		pipelineConfig.renderPass = rocketSwapChain.getRenderPass(); // Default render pass
		pipelineConfig.pipelineLayout = pipelineLayout;
		rocketPipeline = std::make_unique<RocketPipeline>(rocketDevice, vertShaderPath, fragShaderPath, pipelineConfig);
	}
	void TutorialApp::createCommandBuffers()
	{
		commandBuffers.resize(rocketSwapChain.imageCount()); // Either 2 or 3

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = rocketDevice.getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary can be submitted to a queue for execution, but cannot be called from other command buffers
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rocketDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		// Record drawing commands into each command buffer
		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer!");
			}

			// Start the render pass
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rocketSwapChain.getRenderPass();
			renderPassInfo.framebuffer = rocketSwapChain.getFrameBuffer(i);
			// Render area is entire swap chain extent
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = rocketSwapChain.getSwapChainExtent();

			// Clear values for all attachments
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; // Color buffer, depthStencil on index 0 would be ingored because how we strucutred our render pass
			clearValues[1].depthStencil = { 1.0f, 0 }; // Depth buffer
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 
			// VK_SUBPASS_CONTENTS_INLINE: Render pass commands will be embedded in the primary command buffer, and no secondary command buffers will be executed
			rocketPipeline.get()->bind(commandBuffers[i]);
			//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			rocketModel -> bind(commandBuffers[i]);
			rocketModel -> draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS ){
				throw std::runtime_error("Failed to record command buffer!");
			}
		}

	}
	void TutorialApp::drawFrame()
	{
		uint32_t imageIndex; // Current frame buffer
		auto result = rocketSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		result = rocketSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex); // Submit command buffer to queue
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}
	}
	void TutorialApp::loadModels()
	{
		std::vector<RocketModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f , 0.0f}},
			{{0.5f, 0.5f} , {0.0f, 1.0f , 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f , 1.0f}}
		};

		rocketModel = std::make_unique<RocketModel>(rocketDevice, vertices);
	}
}
