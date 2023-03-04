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
		createPipeline();

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

			if (auto commandBuffer = rocketRenderer.beginFrame()) {
				rocketRenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
				ImDrawData* draw_data = ImGui::GetDrawData();
				ImGui_ImplVulkan_RenderDrawData(draw_data, rocketRenderer.getCurrentCommandBuffer());
				rocketRenderer.endSwapChainRenderPass(commandBuffer);
				rocketRenderer.endFrame();
			}
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
		assert(pipelineLayout != nullptr && "Pipeline layout must be created before pipeline!");
		PipelineConfigInfo pipelineConfig{};
		RocketPipeline::defaultPipelineConfigInfo(pipelineConfig); // Swap chain width and height not always equal to screens
		pipelineConfig.renderPass = rocketRenderer.getSwapChainRenderPass(); // Default render pass
		pipelineConfig.pipelineLayout = pipelineLayout;
		rocketPipeline = std::make_unique<RocketPipeline>(rocketDevice, vertShaderPath, fragShaderPath, pipelineConfig);
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

}
