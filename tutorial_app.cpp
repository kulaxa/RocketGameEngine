#include "tutorial_app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>
#include <particle.hpp>
#include <physics_system.hpp>
#include <random>

namespace rocket {
	static std::default_random_engine generator;
	TutorialApp::TutorialApp()
	{
		loadGameObjects();
	}
	TutorialApp::~TutorialApp()
	{
	}
	void TutorialApp::run()
	{
		std::cout << "Starting Tutorial App." << std::endl;
		SimpleRenderSystem simpleRenderSystem(rocketDevice, rocketRenderer.getSwapChainRenderPass());

		//uint32_t testBallPosition = createParticle({ 0.f, 0.f });
		//gameObjects[testBallPosition].acceleration = glm::vec2(0.0f, 2.0f);
		while (!rocketWindow.shouldClose()) {
			glfwPollEvents();

			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static int i = 0;
				static int particleCounter = 0;

				ImGui::Begin("Fps and slider!");                          // Create a window called "Hello, world!" and append into it.

				
				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

				ImGui::SliderInt("Number of particles to add", &i,0, 100);            // Edit 1 float using a slider from 0.0f to 1.0f

				if (ImGui::Button("Clear Simulation")) {
					clearSimulation();
					particleCounter = 0;
				}                        // Buttons return true when clicked (most widgets return true when edited/activated)
				float mouseX = 2 * (ImGui::GetMousePos().x / WIDTH - 0.5f);
				float mouseY = 2 * (ImGui::GetMousePos().y / HEIGHT - 0.5f);
				//glm::vec2 testPaticlePosition = gameObjects[testBallPosition].transform2d.translation;
				//float testPaticleRadius = gameObjects[testBallPosition].radius;
				if (ImGui::IsMouseClicked(1)) {
					
					for (int j = 0; j < i; j++) {
						createParticle({ mouseX, mouseY });
						particleCounter++;
					}
				}
				if (ImGui::IsMouseDown(0)) {
					uint32_t selectedParticle = getSelectedParticle(mouseX, mouseY);
					if (selectedParticle != -1) {
						gameObjects[getParticleIndex(selectedParticle)].transform2d.translation = {mouseX, mouseY};

					}
				}
				ImGui::Text("counter = %d", particleCounter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::Text("Mouse position is %.3f x, %0.3f y", mouseX, mouseY);
				//ImGui::Text("Test particle position is %.3f x, %0.3f y", gameObjects[testBallPosition].transform2d.translation.x, gameObjects[testBallPosition].transform2d.translation.y);


				ImGui::End();
			}

			// Imgui render
			ImGui::Render();

			if (auto commandBuffer = rocketRenderer.beginFrame()) {
				rocketRenderer.beginSwapChainRenderPass(commandBuffer);
				physicsSystem.updatePhysics(1/ ImGui::GetIO().Framerate, gameObjects);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
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


	void TutorialApp::loadGameObjects()
	{

		auto verticies = Particle::createParticleVerticies(0.01f, {0.0f, 0.0f});
		circleModel = std::make_shared<RocketModel>(rocketDevice, verticies);
		
	}

	uint32_t TutorialApp::createParticle(glm::vec2 position)
	{

		std::uniform_real_distribution<double> distribution(-0.1, 0.1);
		RocketGameObject gameObject = RocketGameObject::createGameObject();
		gameObject.model = circleModel;
		gameObject.color = { 40, 40, 40 };
		gameObject.mass = 1.0f;
		gameObject.gravityApplied = true;
		gameObject.collisionApplied = true;
		gameObject.type = RocketGameObjectType::PARTICLE;
		gameObject.radius = 0.01f;
		gameObject.acceleration = { 0.0f, 3.f };
		gameObject.transform2d.translation = {position.x + distribution(generator), position.y +distribution(generator)};

		//gameObject.velocity = { distribution(generator),  distribution(generator) };
		gameObject.velocity = { 0.0f, 0.0f };
		gameObjects.push_back(std::move(gameObject));
		//return std::make_unique<RocketGameObject>(gameObjects.back());
		return gameObjects.size() - 1;
	}

	uint32_t TutorialApp::getSelectedParticle(float xMouse, float yMouse)
	{
		for (auto& particle : gameObjects) {
			float xPart = particle.transform2d.translation.x;
			float yPart = particle.transform2d.translation.y;

			if (xMouse > xPart - particle.radius && xMouse < xPart + particle.radius &&
				yMouse > yPart - particle.radius && yMouse < yPart + particle.radius) {
				return particle.getId();
			}
		}
		return -1;
	}

	uint32_t TutorialApp::getParticleIndex(uint32_t particleId)
	{
		uint32_t counter = 0;
		for (auto& particle : gameObjects) {
			if (particle.getId() == particleId) {
				return counter;
			}
			counter++;
		}
	}

	void TutorialApp::clearSimulation()
	{
		gameObjects.clear();
	}


}
