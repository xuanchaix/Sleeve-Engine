#include "Graphics/Descriptor.h"
#include "Graphics/GraphicsFwd.h"
#include "Core/EngineCommon.h"
#include "Graphics/Renderer.h"

constexpr uint32_t MAX_DESCRIPTOR_IN_POOL = 1024;

DescriptorPools::DescriptorPools( VkDevice device, uint8_t numOfUniformBuffers, uint8_t numOfSamplers )
	:m_device( device ), m_numOfUniformBuffers(numOfUniformBuffers), m_numOfSamplers(numOfSamplers)
{
	m_pools.resize( MAX_FRAMES_IN_FLIGHT );

	VkDescriptorPoolSize poolSize = {};
	m_poolSizes.reserve( 2 );
	if (m_numOfUniformBuffers > 0) {
		poolSize.descriptorCount = (uint32_t)m_numOfUniformBuffers;
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		m_poolSizes.push_back( poolSize );
	}
	if (numOfSamplers > 0) {
		poolSize.descriptorCount = (uint32_t)numOfSamplers;
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		m_poolSizes.push_back( poolSize );
	}
}

DescriptorPools::~DescriptorPools()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		for (auto pool : m_pools[i]) {
			vkDestroyDescriptorPool( m_device, pool, nullptr );
		}
	}
}

void DescriptorPools::BeginFrame()
{
	// free all sets, they need to be reallocated this frame
	for (auto pool : m_pools[g_theRenderer->m_currentFrame]) {
		vkResetDescriptorPool( m_device, pool, 0 );
	}
	m_curIndex = 0;
}

void DescriptorPools::CreateNewDescriptorPool()
{
	VkDescriptorPool newPool = VK_NULL_HANDLE;
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
	poolInfo.pPoolSizes = m_poolSizes.data();
	poolInfo.maxSets = MAX_DESCRIPTOR_IN_POOL;
	ASSERT_OR_ERROR( vkCreateDescriptorPool( m_device, &poolInfo, nullptr, &newPool ) == VK_SUCCESS, "failed to create descriptor pool!" );

	m_pools[g_theRenderer->m_currentFrame].push_back( newPool );
}

VkDescriptorPool DescriptorPools::GetDescriptorPool( uint32_t curFrame, uint32_t index ) const
{
	return m_pools[curFrame][index];
}

VkDescriptorSet DescriptorPools::AcquireDescriptorSet( VkDescriptorSetLayout const& layout )
{
	// If has no descriptor pool, a new one is needed to be created
	if (m_pools[g_theRenderer->m_currentFrame].empty()) {
		CreateNewDescriptorPool();
	}

CreateDescriptorSet: // If creation is failed, return to this line
	// Get current descriptor pool
	VkDescriptorPool pool = GetDescriptorPool( g_theRenderer->m_currentFrame, m_curIndex );

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;

	// Allocate a new descriptor set
	VkDescriptorSet descriptorSet;
	VkResult result = vkAllocateDescriptorSets( m_device, &allocInfo, &descriptorSet );

	// If there is enough space, return the new one
	if (result == VK_SUCCESS) {
		return descriptorSet;
	}
	// If the pool is full, create a new pool and retry
	else if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
		CreateNewDescriptorPool();
		++m_curIndex;
		goto CreateDescriptorSet;
	}
	return VK_NULL_HANDLE;
}
