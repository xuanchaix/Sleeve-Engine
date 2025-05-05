#pragma once
#include <vulkan/vulkan.h>

class VertexBuffer {
public:
	~VertexBuffer()
	{
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_deviceMemory, nullptr );
	};
protected:
	friend class Renderer;
	VertexBuffer( VkDevice device, uint32_t vertexCount ) :m_device( device ), m_vertexCount( vertexCount ) {};
	uint32_t m_vertexCount = 0;
	uint32_t m_stride = 0;
	uint64_t m_maxSize = 0;
	VkDevice m_device = nullptr;
	VkBuffer m_buffer = nullptr;
	VkDeviceMemory m_deviceMemory = nullptr;
};
