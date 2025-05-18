#pragma once
#include <vulkan/vulkan.h>
#include "Graphics/GraphicsCommon.h"

class StagingBuffer {
	friend class Renderer;
	StagingBuffer( VkDevice device, uint64_t maxSize );
	~StagingBuffer();

	void Refresh();
	bool FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size );
	void ReturnMemory( uint64_t offset, uint64_t size );

	VkBuffer m_stagingBuffer;
	VkDeviceMemory m_stagingBufferMemory;
	void* m_mappedData = nullptr;
	uint8_t* m_stagingPtr = nullptr;
	VkDevice m_device;
	std::vector<BufferMemoryBlock> m_memoryBlocks;
	uint64_t m_maxSize = 0;
};