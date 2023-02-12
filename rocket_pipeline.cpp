#include "rocket_pipeline.hpp"
#include "rocket_model.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
namespace rocket {



	RocketPipeline::RocketPipeline(RocketDevice& device, 
		const std::string& vertFilePath, 
		const std::string& fragFilePath, 
		const PipelineConfigInfo pipelineConfigInfo): rocketDevice{device}
	{
		createGraphicsPipeline(vertFilePath, fragFilePath, pipelineConfigInfo);
		std::cout << "Graphics pipeline created." << std::endl;
	}

	RocketPipeline::~RocketPipeline() {
		vkDestroyShaderModule(rocketDevice.device(), fragShaderModule, nullptr);
		vkDestroyShaderModule(rocketDevice.device(), vertShaderModule, nullptr);
		vkDestroyPipeline(rocketDevice.device(), graphicsPipeline, nullptr);
	}

	void RocketPipeline::bind(VkCommandBuffer commandBuffer)
	{
		// Bind the pipeline to the command buffer
		// This is graphics pipeline, so we use VK_PIPELINE_BIND_POINT_GRAPHICS
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	PipelineConfigInfo RocketPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};
		// Assembly stage - using list of triangled, each 6 numbers is one triagle
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// Viewport - picture goes from (0,0) to (width, height)
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;
		// Everything outside scissors will be discraded
		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };

		// Rasterizer - takes geometry and turns it into fragments to be colored by the fragment shader
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // Z values can be whatever they want
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // We can discard all triagles that are facing away from us
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		// Multisampling - can be used for anti-aliasing, used for smoothening edges of triangles
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		// Color blending - how to blend a new color with the color that is already in the framebuffer
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional


		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		// Depth buffer config
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional
		return configInfo;
	}

	// Read a file into a vector of chars
	std::vector<char> RocketPipeline::readFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary }; // Start at the end of the file

		if(!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg()); // Already at the end of the file
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	// Create the graphics pipeline using given vertex and fragment shader files
	void RocketPipeline::createGraphicsPipeline(
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		PipelineConfigInfo pipelineConfigInfo)
	{
		assert(pipelineConfigInfo.pipelineLayout != VK_NULL_HANDLE
			&& "Cannot create grahpics pipeline: no pipelineLayout provided in configInfo!");
		assert(pipelineConfigInfo.renderPass != VK_NULL_HANDLE
			&& "Cannot create grahpics pipeline: no pipelineLayout provided in configInfo!");
		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pSpecializationInfo = nullptr;  // Optional
		shaderStages[0].pNext = nullptr;               // Optional

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pSpecializationInfo = nullptr;  // Optional
		shaderStages[1].pNext = nullptr;               // Optional

		// Vertex input config
		auto bindingDescription = RocketModel::Vertex::getBindingDescriptions();
		auto attributeDescriptions = RocketModel::Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();  // Optional
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();  // Optional

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &pipelineConfigInfo.viewport;  // Optional
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &pipelineConfigInfo.scissor;  // Optional

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2; // How many programable stages to use
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &pipelineConfigInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &pipelineConfigInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &pipelineConfigInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &pipelineConfigInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &pipelineConfigInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;  // Optional

		pipelineInfo.layout = pipelineConfigInfo.pipelineLayout;
		pipelineInfo.renderPass = pipelineConfigInfo.renderPass;
		pipelineInfo.subpass = pipelineConfigInfo.subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;;  // Optional
		pipelineInfo.basePipelineIndex = -1;    // Optional

		if(vkCreateGraphicsPipelines(rocketDevice.device(), 
			VK_NULL_HANDLE, 
			1, 
			&pipelineInfo, 
			nullptr, 
			&graphicsPipeline) 
				!= VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");

	}

	void RocketPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(rocketDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}
	}

}
