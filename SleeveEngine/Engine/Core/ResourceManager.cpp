#include "Core/ResourceManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Font.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	delete m_whiteTexture;
	for (auto& texturePair : m_textures) {
		delete texturePair.second;
	}
	for (auto& shaderPair : m_shaders) {
		delete shaderPair.second;
	}
	for (auto& fontPair : m_fonts) {
		delete fontPair.second;
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

Texture* ResourceManager::GetWhiteTexture()
{
	if (m_whiteTexture) {
		return m_whiteTexture;
	}
	else {
		m_whiteTexture = g_theRenderer->CreateWhiteTexture();
		return m_whiteTexture;
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

Font* ResourceManager::GetOrLoadFont( std::string const& path )
{
	auto iter = m_fonts.find( path );
	if (iter == m_fonts.end()) {
		Font* newFont = new Font( path );
		m_fonts[path] = newFont;
		return newFont;
	}
	else {
		return iter->second;
	}
}

