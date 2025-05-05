#pragma once
#include <vulkan/vulkan.h>

class IndexBuffer {
public:
	~IndexBuffer()
	{
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_deviceMemory, nullptr );
	};
protected:
	friend class Renderer;
	IndexBuffer( VkDevice device, uint32_t indexCount ) :m_device( device ), m_indexCount( indexCount ) {};

	uint64_t m_maxSize = 0;
	uint32_t m_indexCount = 0;
	VkDevice m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_deviceMemory;
};
