#include "Graphics/Texture.h"

Texture::~Texture()
{
	vkDestroyImageView( m_device, m_textureImageView, nullptr );
	vkDestroyImage( m_device, m_textureImage, nullptr );
	vkFreeMemory( m_device, m_textureDeviceMemory, nullptr );
}
