#pragma once

#include <map>
#include <string>

class Texture;
class Shader;
class Font;

class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();
	Texture* GetOrLoadTexture( std::string const& path );
	Texture* GetWhiteTexture();
	Shader* GetOrLoadShader( std::string const& shaderName );

protected:
	Texture* m_whiteTexture = nullptr;
	std::map<std::string, Texture*> m_textures;
	std::map<std::string, Shader*> m_shaders;
	std::map<std::string, Font*> m_fonts;
};