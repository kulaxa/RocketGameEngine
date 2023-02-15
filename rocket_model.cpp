#include "rocket_model.hpp"
#include <cassert>
#include <iostream>

namespace rocket {
	RocketModel::RocketModel(RocketDevice& device, const std::vector<Vertex>& vertices) : rocketDevice {device}
	{
		createVertexBuffers(vertices);
	}

	RocketModel::~RocketModel()
	{
		vkDestroyBuffer(rocketDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(rocketDevice.device(), vertexBufferMemory, nullptr);
	}

	void RocketModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets); // Record command to bind vertex buffer

	}

	void RocketModel::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

	}

	void RocketModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		rocketDevice.createBuffer(bufferSize, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // memory can be seen by CPU and GPU
			vertexBuffer, 
			vertexBufferMemory);

		void* data;
		vkMapMemory(rocketDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data); // copy data to CPU
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize)); // automatically copy it to GPU
		vkUnmapMemory(rocketDevice.device(), vertexBufferMemory); // remove data from CPU
	}

	std::vector<VkVertexInputBindingDescription> RocketModel::Vertex::getBindingDescriptions()
	{
		
		std::vector<VkVertexInputBindingDescription> bindingDescription(1);
		bindingDescription[0].binding = 0;
		bindingDescription[0].stride = sizeof(Vertex);
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> RocketModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescirptions(2);
		vertexAttributeDescirptions[0].location = 0;
		vertexAttributeDescirptions[0].format = VK_FORMAT_R32G32_SFLOAT; // vec2 in shader
		vertexAttributeDescirptions[0].offset = offsetof(Vertex, position);
		vertexAttributeDescirptions[0].binding = 0;
		
		vertexAttributeDescirptions[1].location = 1;
		vertexAttributeDescirptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3 in shader
		vertexAttributeDescirptions[1].offset = offsetof(Vertex, color);
		vertexAttributeDescirptions[1].binding = 0;

		return vertexAttributeDescirptions;
	}

}
