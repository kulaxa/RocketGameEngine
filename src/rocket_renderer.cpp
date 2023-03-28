#include "rocket_pipeline.hpp"
#include "rocket_renderer.hpp"

#include <iostream>
#include <stdexcept>
#include <array>

namespace rocket {

	RocketRenderer::RocketRenderer(RocketWindow& window, RocketDevice& device) : rocketWindow(window), rocketDevice(device)
	{
		recreateSwapChain();
		createCommandBuffers();
		setupImGui();
	}
	


	
	RocketRenderer::~RocketRenderer()
	{
		freeCommandBuffers();
	}
	

	VkCommandBuffer RocketRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cannot start frame before ending previous frame");

		auto result = rocketSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}


		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void RocketRenderer::endFrame()
	{
		assert(isFrameStarted && "Cannot end frame before starting frame");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = rocketSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex); // Submit command buffer to queue
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rocketWindow.wasWindowResized())
		{
			rocketWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % RocketSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void RocketRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot record command buffer outside of a frame");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot record command buffer in another frame");

		// Start the render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rocketSwapChain->getRenderPass();
		renderPassInfo.framebuffer = rocketSwapChain->getFrameBuffer(currentImageIndex);
		// Render area is entire swap chain extent
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = rocketSwapChain->getSwapChainExtent();

		// Clear values for all attachments
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; // Color buffer, depthStencil on index 0 would be ingored because how we strucutred our render pass
		clearValues[1].depthStencil = { 1.0f, 0 }; // Depth buffer
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)rocketSwapChain->getSwapChainExtent().width;
		viewport.height = (float)rocketSwapChain->getSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = rocketSwapChain->getSwapChainExtent();

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void RocketRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass outside of a frame");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end swap chain in another frame");


		vkCmdEndRenderPass(commandBuffer);
	}

	void RocketRenderer::createCommandBuffers()
	{
		commandBuffers.resize(RocketSwapChain::MAX_FRAMES_IN_FLIGHT); // Either 2 or 3

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = rocketDevice.getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary can be submitted to a queue for execution, but cannot be called from other command buffers
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rocketDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}

	}
	void RocketRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(rocketDevice.device(),
			rocketDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}
	void RocketRenderer::recreateSwapChain()
	{
		auto extent = rocketWindow.getExtent();
		// Check for minimized window
		while (extent.width == 0 || extent.height == 0) {
			extent = rocketWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rocketDevice.device());
		//ImGui_ImplVulkan_InitInfo initInfo {};
		//rocketDevice.initDeviceImgui(0, initInfo);
		//ImGui_ImplVulkanH_CreateOrResizeWindow(initInfo.Instance, initInfo.PhysicalDevice, initInfo.Device, nullptr, initInfo.QueueFamily, nullptr, extent.width, extent.height, initInfo.MinImageCount);
		//assert(rocketSwapChain != nullptr && "Swap chain must be created before pipeline!");

		if (rocketSwapChain == nullptr) {
			rocketSwapChain = std::make_unique<RocketSwapChain>(rocketDevice, extent);
		}
		else {
			std::shared_ptr<RocketSwapChain> oldSwapChain = std::move(rocketSwapChain);
			rocketSwapChain = std::make_unique<RocketSwapChain>(rocketDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*rocketSwapChain)) {
				throw std::runtime_error("Swap chain image or depth has changed!");
			}
		}
	}
	

	void RocketRenderer::setupImGui()
	{
		//Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(rocketWindow.getWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		rocketDevice.initDeviceImgui(rocketSwapChain->imageCount(), init_info);
		ImGui_ImplVulkan_Init(&init_info, rocketSwapChain->getRenderPass());

		// Upload fonts
		VkCommandBuffer command_buffer = rocketDevice.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		rocketDevice.endSingleTimeCommands(command_buffer);



	}

}
