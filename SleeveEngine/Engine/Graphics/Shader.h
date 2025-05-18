#pragma once
#include "GraphicsFwd.h"
#include <vector>
#include <string>

class Renderer;
class DescriptorPools;
struct Legacy_EntityUniformBuffers;
struct UniformBufferBinding;

class Shader {
public:
	void UpdateDescriptorSets( Legacy_EntityUniformBuffers const& uniformBuffers );
	void UpdateDescriptorSets( UniformBufferBinding const& binding );

protected:
	friend class Renderer;
	friend class ResourceManager;

	Shader( VkDevice device, Renderer* renderer ) : m_device( device ), m_renderer(renderer) {};
	~Shader();

	void LoadShader( std::string const& fileName );

	void CreateDescriptorSetLayout();

	void CreateGraphicsPipeline();

	static std::vector<char> ReadFile( const std::string& filename );

	VkShaderModule CreateShaderModule( const std::vector<char>& code );

	std::string m_name;
	VkDevice m_device;
	Renderer* m_renderer;
	Texture* m_texture;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;
	DescriptorPools* m_pools = nullptr;
};