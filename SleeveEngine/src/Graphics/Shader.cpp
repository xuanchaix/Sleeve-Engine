#include "Graphics/Shader.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <fstream>
#include <format>

#include "Graphics/GraphicsCommon.h"
#include "Graphics/Renderer.h"
extern PerspectiveCamera* globalCamera;

void Shader::UpdateDescriptorSets( EntityUniformBuffers const& uniformBuffers )
{
	// Get a descriptor set, all acquired sets will be freed next frame
	VkDescriptorSet set = m_pools->AcquireDescriptorSet( m_descriptorSetLayout );

	VkDescriptorBufferInfo vpBufferInfo{};
	vpBufferInfo.buffer = globalCamera->m_cameraUniformBuffers[m_renderer->GetCurFrameNumber()]->m_buffer;
	vpBufferInfo.offset = 0;
	vpBufferInfo.range = sizeof( CameraUniformBufferObject );

	VkDescriptorBufferInfo modelBufferInfo{};
	modelBufferInfo.buffer = uniformBuffers.m_uniformBuffersModel[m_renderer->GetCurFrameNumber()]->m_buffer;
	modelBufferInfo.offset = 0;
	modelBufferInfo.range = sizeof( ModelUniformBufferObject );

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_texture->m_textureImageView;
	imageInfo.sampler = m_renderer->m_textureSampler;

	std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = set;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &vpBufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = set;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &modelBufferInfo;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = set;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &imageInfo;

	// bind the set
	vkCmdBindDescriptorSets( g_theRenderer->m_commandBuffers[g_theRenderer->m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &set, 0, nullptr );
	// update the set
	vkUpdateDescriptorSets( m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr );
}

void Shader::UpdateDescriptorSets( UniformBufferBinding const& binding )
{
	// Get a descriptor set, all acquired sets will be freed next frame
	VkDescriptorSet set = m_pools->AcquireDescriptorSet( m_descriptorSetLayout );

	VkDescriptorBufferInfo vpBufferInfo{};
	vpBufferInfo.buffer = globalCamera->m_cameraUniformBuffers[m_renderer->GetCurFrameNumber()]->m_buffer;
	vpBufferInfo.offset = 0;
	vpBufferInfo.range = sizeof( CameraUniformBufferObject );

	VkDescriptorBufferInfo modelBufferInfo{};
	modelBufferInfo.buffer = m_renderer->m_sharedModelUniformBuffers[m_renderer->GetCurFrameNumber()]->m_buffer;
	modelBufferInfo.offset = binding.m_modelUniformBufferOffset;
	modelBufferInfo.range = sizeof( ModelUniformBufferObject );

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_texture->m_textureImageView;
	imageInfo.sampler = m_renderer->m_textureSampler;

	std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = set;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &vpBufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = set;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &modelBufferInfo;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = set;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &imageInfo;

	// bind the set
	vkCmdBindDescriptorSets( g_theRenderer->m_commandBuffers[g_theRenderer->m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &set, 0, nullptr );
	// update the set
	vkUpdateDescriptorSets( m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr );
}

Shader::~Shader()
{
	vkDestroyDescriptorSetLayout( m_device, m_descriptorSetLayout, nullptr );
	vkDestroyPipeline( m_device, m_graphicsPipeline, nullptr );
	vkDestroyPipelineLayout( m_device, m_pipelineLayout, nullptr );
}

void Shader::LoadShader( std::string const& fileName )
{
	m_name = fileName;
	m_texture = g_theResourceManager->GetOrLoadTexture( "Data/Textures/texture.png" );
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	m_pools = g_theRenderer->GetOrCreateDescriptorPools( 2, 1 );
}

void Shader::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding cameraUboLayoutBinding{};
	cameraUboLayoutBinding.binding = 0;
	cameraUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	cameraUboLayoutBinding.descriptorCount = 1;
	cameraUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	cameraUboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding modelUboLayoutBinding{};
	modelUboLayoutBinding.binding = 1;
	modelUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	modelUboLayoutBinding.descriptorCount = 1;
	modelUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	modelUboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 3> bindings = { cameraUboLayoutBinding, modelUboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout( m_device, &layoutInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create descriptor set layout!" );
	}
}


/// (Vulkan) get binding description of the vertex buffer
static VkVertexInputBindingDescription GetBindingDescriptionVertexPCU3D() 
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof( VertexPCU3D );
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

/// (Vulkan) get attribute description of the vertex in the shader
static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptionsVertexPCU3D() 
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof( VertexPCU3D, m_position );

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R8G8B8A8_UNORM;
	attributeDescriptions[1].offset = offsetof( VertexPCU3D, m_color );

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof( VertexPCU3D, m_uvCoord );

	return attributeDescriptions;
}

void Shader::CreateGraphicsPipeline()
{
	auto vertShaderCode = ReadFile( std::format("Data/Shaders/{}_vert.spv", m_name) );
	auto fragShaderCode = ReadFile( std::format("Data/Shaders/{}_frag.spv", m_name) );
	VkShaderModule vertShaderModule = CreateShaderModule( vertShaderCode );
	VkShaderModule fragShaderModule = CreateShaderModule( fragShaderCode );

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = GetBindingDescriptionVertexPCU3D();
	auto attributeDescriptions = GetAttributeDescriptionsVertexPCU3D();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_renderer->m_swapChainExtent.width;
	viewport.height = (float)m_renderer->m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_renderer->m_swapChainExtent;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	//viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	//viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_TRUE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout( m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create pipeline layout!" );
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderer->m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines( m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create graphics pipeline!" );
	}

	vkDestroyShaderModule( m_device, fragShaderModule, nullptr );
	vkDestroyShaderModule( m_device, vertShaderModule, nullptr );
}

VkShaderModule Shader::CreateShaderModule( const std::vector<char>& code )
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule( m_device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create shader module!" );
	}
	return shaderModule;
}

std::vector<char> Shader::ReadFile( const std::string& filename )
{
	std::ifstream file( filename, std::ios::ate | std::ios::binary );

	if (!file.is_open()) {
		throw std::runtime_error( "failed to open file!" );
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer( fileSize );
	file.seekg( 0 );
	file.read( buffer.data(), fileSize );
	file.close();

	return buffer;
}
