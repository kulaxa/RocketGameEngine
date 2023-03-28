#pragma once

#include "rocket_window.hpp"
#include "rocket_device.hpp"
#include "rocket_swap_chain.hpp"
#include <memory>
#include <vector>
#include <cassert>
#include "rocket_model.hpp"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"


namespace rocket {

	class RocketRenderer {
	public:
		RocketRenderer(RocketWindow& window, RocketDevice& device);
		~RocketRenderer();

		RocketRenderer(const RocketRenderer&) = delete;
		RocketRenderer& operator=(const RocketRenderer&) = delete; // Disable copying TutorialApp

		bool isFrameInProgress() const {return isFrameStarted;}
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer before frame is started");
			return commandBuffers[currentFrameIndex];}
		VkRenderPass getSwapChainRenderPass() const {return rocketSwapChain->getRenderPass();}

		int gameFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index before frame is started");
			return currentFrameIndex;
		}
		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		void setupImGui();

		RocketWindow& rocketWindow;
		RocketDevice& rocketDevice;
		std::unique_ptr<RocketSwapChain> rocketSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted = false;
	};
}