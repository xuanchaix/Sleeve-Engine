#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "GraphicsCommon.h"

class VertexBuffer {
public:
	~VertexBuffer()
	{
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_deviceMemory, nullptr );
	};

public:
protected:
	friend class Renderer;
	VertexBuffer( VkDevice device, uint64_t size );
	bool FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size );
	void EnlargeBuffer( uint64_t newSize );
	void ReturnMemory( uint64_t offset, uint64_t size );
	uint32_t m_stride = 0;
	uint64_t m_maxSize = 0;
	VkDevice m_device = nullptr;
	VkBuffer m_buffer = nullptr;
	VkDeviceMemory m_deviceMemory = nullptr;
	std::vector<BufferMemoryBlock> m_memoryBlocks;
};
