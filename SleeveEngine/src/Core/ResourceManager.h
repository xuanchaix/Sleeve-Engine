#pragma once

#include <map>
#include <string>

class Texture;
class Shader;

class ResourceManager {
public:
	~ResourceManager();
	Texture* GetOrLoadTexture( std::string const& path );
	Shader* GetOrLoadShader( std::string const& shaderName );

protected:
	std::map<std::string, Texture*> m_textures;
	std::map<std::string, Shader*> m_shaders;
};