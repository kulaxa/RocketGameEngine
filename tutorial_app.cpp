#include "tutorial_app.hpp"
#include "rocket_pipeline.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>

namespace rocket {
	// This is temp
	struct SimplePushConstantData {
		glm::mat2 transform{ 0.5f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	TutorialApp::TutorialApp()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
		setupImGui();

	}
	TutorialApp::~TutorialApp()
	{
		vkDestroyPipelineLayout(rocketDevice.device(), pipelineLayout, nullptr);
	}
	void TutorialApp::run()
	{
		std::cout << "Starting Tutorial App." << std::endl;

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		while (!rocketWindow.shouldClose()) {
			glfwPollEvents();

			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			// Imgui render
			ImGui::Render();

			drawFrame();
		}
		vkDeviceWaitIdle(rocketDevice.device());
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDeviceWaitIdle(rocketDevice.device());

		vkDestroyDescriptorPool(rocketDevice.device(), rocketDevice.getDescriptorPool(), nullptr);


	}
	void TutorialApp::createPipelineLayout()
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
	void TutorialApp::createPipeline()
	{
		assert(rocketSwapChain != nullptr && "Swap chain must be created before pipeline!");
		assert(pipelineLayout != nullptr && "Pipeline layout must be created before pipeline!");
		PipelineConfigInfo pipelineConfig{};
		RocketPipeline::defaultPipelineConfigInfo(pipelineConfig); // Swap chain width and height not always equal to screens
		pipelineConfig.renderPass = rocketSwapChain -> getRenderPass(); // Default render pass
		pipelineConfig.pipelineLayout = pipelineLayout;
		rocketPipeline = std::make_unique<RocketPipeline>(rocketDevice, vertShaderPath, fragShaderPath, pipelineConfig);
	}
	void TutorialApp::createCommandBuffers()
	{
		commandBuffers.resize(rocketSwapChain -> imageCount()); // Either 2 or 3

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = rocketDevice.getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary can be submitted to a queue for execution, but cannot be called from other command buffers
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rocketDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}

	}
	void TutorialApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(rocketDevice.device(), 
			rocketDevice.getCommandPool() , 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data());
		commandBuffers.clear();
	}
	void TutorialApp::drawFrame()
	{
		uint32_t imageIndex; // Current frame buffer
		auto result = rocketSwapChain -> acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}


		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
		recordCommandBuffer(imageIndex);


		result = rocketSwapChain-> submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex); // Submit command buffer to queue
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rocketWindow.wasWindowResized())
		{
			rocketWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}
	}
	void TutorialApp::loadGameObjects()
	{
		std::vector<RocketModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f , 0.0f}},
			{{0.5f, 0.5f} , {0.0f, 1.0f , 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f , 1.0f}}
		};

		auto rocketModel = std::make_shared<RocketModel>(rocketDevice, vertices); // one model for multiple game objects

		auto triangle = RocketGameObject::createGameObject();
		triangle.model = rocketModel;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.0f, 0.5f };
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>(); // 90 degrees, using radians
		gameObjects.push_back(std::move(triangle));
	}
	void TutorialApp::recreateSwapChain()
	{
		auto extent = rocketWindow.getExtent();
		// Check for minimized window
		while (extent.width == 0 || extent.height == 0) {
			extent = rocketWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rocketDevice.device());

		if (rocketSwapChain == nullptr) {
			rocketSwapChain = std::make_unique<RocketSwapChain>(rocketDevice, extent);
		}
		else {
			rocketSwapChain = std::make_unique<RocketSwapChain>(rocketDevice, extent, std::move(rocketSwapChain));
			if (rocketSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		// If pipeline is compatable with new swap chain, no need to recreate
		createPipeline();
	}
	void TutorialApp::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		// Start the render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rocketSwapChain->getRenderPass();
		renderPassInfo.framebuffer = rocketSwapChain->getFrameBuffer(imageIndex);
		// Render area is entire swap chain extent
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = rocketSwapChain->getSwapChainExtent();

		// Clear values for all attachments
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; // Color buffer, depthStencil on index 0 would be ingored because how we strucutred our render pass
		clearValues[1].depthStencil = { 1.0f, 0 }; // Depth buffer
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		ImDrawData* draw_data = ImGui::GetDrawData();
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		// VK_SUBPASS_CONTENTS_INLINE: Render pass commands will be embedded in the primary command buffer, and no secondary command buffers will be executed
		renderGameObjects(commandBuffers[imageIndex]);
		ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
	void TutorialApp::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		rocketPipeline ->bind(commandBuffer);
		for (auto& gameObject : gameObjects) {
			gameObject.transform2d.rotation = glm::mod(gameObject.transform2d.rotation + 0.01f, glm::two_pi<float>()); // 90 degrees, using radians

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
			gameObject.model ->bind(commandBuffer);
			gameObject.model ->draw(commandBuffer);

		}
	}
	void TutorialApp::setupImGui()
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
		ImGui_ImplGlfw_InitForVulkan(rocketWindow.getWindow(),  true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		rocketDevice.initDeviceImgui(rocketSwapChain->imageCount(), init_info);
		ImGui_ImplVulkan_Init(&init_info, rocketSwapChain-> getRenderPass());

		// Upload fonts
		VkCommandBuffer command_buffer = rocketDevice.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		rocketDevice.endSingleTimeCommands(command_buffer);

		

	}

}
