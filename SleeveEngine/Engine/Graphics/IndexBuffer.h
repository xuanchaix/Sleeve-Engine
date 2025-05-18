#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Graphics/GraphicsCommon.h"

class IndexBuffer {
public:
	~IndexBuffer()
	{
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_deviceMemory, nullptr );
	};
protected:
	friend class Renderer;
	IndexBuffer( VkDevice device, uint32_t indexCount, uint64_t size );;
	bool FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size );
	void EnlargeBuffer( uint64_t newSize );
	void ReturnMemory( uint64_t offset, uint64_t size );

	uint64_t m_maxSize = 0;
	uint32_t m_indexCount = 0;
	VkDevice m_device = nullptr;
	VkBuffer m_buffer = nullptr;
	VkDeviceMemory m_deviceMemory = nullptr;
	std::vector<BufferMemoryBlock> m_memoryBlocks;
};
