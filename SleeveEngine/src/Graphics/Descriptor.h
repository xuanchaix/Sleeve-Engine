#pragma once
#include <vector>
#include <vulkan/vulkan.h>

class DescriptorPools {
	friend class Renderer;
	friend class Shader;
	DescriptorPools( VkDevice device, uint8_t numOfUniformBuffers, uint8_t numOfSamplers );
	~DescriptorPools();

	void BeginFrame();
	void CreateNewDescriptorPool();
	VkDescriptorPool GetDescriptorPool( uint32_t curFrame, uint32_t index ) const;
	VkDescriptorSet AcquireDescriptorSet( VkDescriptorSetLayout const& layout );

	VkDevice m_device;
	uint32_t m_curIndex = 0;
	std::vector<std::vector<VkDescriptorPool>> m_pools;
	std::vector<VkDescriptorPoolSize> m_poolSizes;
	uint8_t m_numOfUniformBuffers = 0;
	uint8_t m_numOfSamplers = 0;
};