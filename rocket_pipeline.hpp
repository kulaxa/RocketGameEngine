#pragma once
#include <string>
#include <vector>
#include "rocket_device.hpp"

namespace rocket {
	struct PipelineConfigInfo {
		//VkViewport viewport; No longer needed because we are using dynamic viewport and scissor
		//VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewport;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;	
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class RocketPipeline {
	public:
		RocketPipeline(RocketDevice &device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo pipelineConfigInfo);
		~RocketPipeline();
		RocketPipeline(const RocketPipeline&) = delete;
		RocketPipeline& operator=(const RocketPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		RocketDevice& rocketDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}
