#pragma once
#include <vulkan/vulkan.h>


class Texture {
	friend class Renderer;
	friend class Shader;
	friend class ResourceManager;
	Texture( VkDevice device ) :m_device(device) { }
	~Texture();
	VkImage m_textureImage = nullptr;
	VkDeviceMemory m_textureDeviceMemory = nullptr;
	VkImageView m_textureImageView = nullptr;
	VkDevice m_device = nullptr;
};