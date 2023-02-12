#pragma once
#include "rocket_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace rocket {
	class RocketModel {
	public:

		struct Vertex
		{
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		};
		RocketModel(RocketDevice &device, const std::vector<Vertex>& vertices);
		~RocketModel();

		RocketModel(const RocketModel&) = delete;
		void operator=(const RocketModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		RocketDevice &rocketDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}