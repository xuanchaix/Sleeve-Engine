#pragma once
#include <vulkan/vulkan.h>

class UniformBuffer {
public:
	~UniformBuffer() {
		vkDestroyBuffer( m_device, m_buffer, nullptr );
		vkFreeMemory( m_device, m_uniformBufferMemory, nullptr );
	}
protected:
	friend class Renderer;
	friend class Shader;
	UniformBuffer( VkDevice device ) : m_device( device ) {};
	VkDevice m_device = nullptr;
	VkBuffer m_buffer = nullptr;
	VkDeviceMemory m_uniformBufferMemory = nullptr;
	void* m_uniformBufferMapped = nullptr;
	uint64_t m_stride = 0;
	uint64_t m_currentOffset = 0;
	uint64_t m_maxSize = 0;
};