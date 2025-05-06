#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Graphics/GraphicsCommon.h"

class UniformBuffer {
public:
	~UniformBuffer() {
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_uniformBufferMemory, nullptr );
	}
protected:
	friend class Renderer;
	friend class Shader;
	UniformBuffer( VkDevice device, uint64_t size );
	bool FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size );
	void ReturnMemory( uint64_t offset, uint64_t size );
	VkDevice m_device = nullptr;
	VkBuffer m_buffer = nullptr;
	VkDeviceMemory m_uniformBufferMemory = nullptr;
	void* m_uniformBufferMapped = nullptr;
	uint64_t m_stride = 0;
	uint64_t m_maxSize = 0;
	std::vector<BufferMemoryBlock> m_memoryBlocks;
};