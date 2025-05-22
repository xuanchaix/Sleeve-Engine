#pragma once
#include <string>
#include <vector>
#include "Graphics/Vertex.h"

class Texture;

class Font {
public:
	Font( std::string const& fontPath );
	~Font();

	void AddVertsForText2D( std::vector<VertexPCU3D>& verts, std::string const& text, Vec2 const& leftBottomPos, Rgba8 const& color, float textSize, float height = 0.f ) const;
	Texture* GetTexture() const;
protected:
	float m_fontSize = 64.f;
	void* m_fontPackRange = nullptr;
	Texture* m_texture;
	std::string m_path;
};