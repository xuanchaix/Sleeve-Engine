#include "Core/ResourceManager.h"
#include "Graphics/Renderer.h"

ResourceManager::~ResourceManager()
{
	for (auto& texturePair : m_textures) {
		delete texturePair.second;
	}
	for (auto& shaderPair : m_shaders) {
		delete shaderPair.second;
	}
}

Texture* ResourceManager::GetOrLoadTexture( std::string const& path )
{
	auto iter = m_textures.find( path );
	if (iter == m_textures.end()) {
		Texture* newTexture = g_theRenderer->CreateTextureFromFile( path );
		m_textures[path] = newTexture;
		return newTexture;
	}
	else {
		return iter->second;
	}
}

Shader* ResourceManager::GetOrLoadShader( std::string const& shaderName )
{
	auto iter = m_shaders.find( shaderName );
	if (iter == m_shaders.end()) {
		Shader* newShader = g_theRenderer->CreateShader( shaderName );
		m_shaders[shaderName] = newShader;
		return newShader;
	}
	else {
		return iter->second;
	}
}

